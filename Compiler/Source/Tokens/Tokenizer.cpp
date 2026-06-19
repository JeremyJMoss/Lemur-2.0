#include "Tokens/Tokenizer.hpp"
#include "Tokens/Token.hpp"

const std::regex Tokenizer::s_TOKEN_PATTERN(
     R"(&&|\|\||==|!=|<=|>=|->|::|=>|'([^']*)'|[0-9]+\.[0-9]+|[0-9]+|/\*|\*/|[\"+\-*/()=;:<>{}%,!.\[\]]|\w+)"
);

const std::regex Tokenizer::s_WHITESPACE( 
    R"(\s+)" 
);

const std::regex Tokenizer::s_RE_INTEGER( 
    R"(^[0-9]+$)" 
);

const std::regex Tokenizer::s_RE_FLOAT( 
    R"(^[0-9]+\.[0-9]+$)" 
);

const std::regex Tokenizer::s_RE_CHAR(
    R"(^'([^']*)'$)"
);

void Tokenizer::resetState() 
{
    m_tokens.clear();
    m_lineNum = 0;
    m_partialToken.clear();
    m_inToken = false;
}

std::vector<Token> Tokenizer::tokenizeFile( const std::string& filePath ) 
{
    resetState();
    size_t fileId = m_srcManager.addFile(filePath);
    
    std::ifstream fileStream( filePath );
    if ( !fileStream.is_open() ) 
    {
        m_errReporter.report( RuntimeError(
                ErrorSeverity::Fatal,
                "Error opening .lmur file"
            ) 
        );
        return m_tokens;
    }
    
    return tokenizeStream( fileStream, fileId );
}

std::vector<Token> Tokenizer::tokenizeStream( std::istream& stream, size_t fileId, bool onlyHeader ) 
{
    std::string line;
    m_tokens.clear();
    m_lineNum = 0;
    size_t pos;
    size_t maxTokens = 20;
    size_t headerTokens = 0;
    clearPartialToken();

    while ( true ) 
    {
        if ( onlyHeader )
        {
            if ( headerTokens >= maxTokens ) break;
        }

        m_lineNum++;
        pos = 0;

        if ( !std::getline( stream, line ) ) break;

        while ( pos < line.length() ) 
        {
            if ( onlyHeader )
            {
                if ( headerTokens >= maxTokens ) break;
            }

            std::smatch wsMatch;
            if ( matchRegex( line, pos, wsMatch, s_WHITESPACE ) ) 
            {
                if ( !m_inToken ) 
                {
                    pos += wsMatch.length();
                    continue;
                } 
                else 
                {
                    std::string value = wsMatch.str( 0 );

                    if ( pos != 0 && pos + wsMatch.length() != line.length() ) 
                    {
                        appendPartialToken( value, m_lineNum, pos + line.length() );
                    } 
                    else if ( pos + wsMatch.length() != line.length() ) 
                    {
                        appendPartialToken( " ", m_lineNum, pos + line.length() );
                    }

                    pos += wsMatch.length();
                    continue;
                }
            }

            std::smatch match;
            if ( matchRegex( line, pos, match, s_TOKEN_PATTERN ) ) 
            {
                std::string value = match.str( 0 );

                if ( m_inToken ) 
                {
                    if ( m_partialToken.getType() == TokenKind::String && value.front() == '"' ) 
                    {
                        appendPartialToken( value, m_lineNum, pos + match.length() );
                        m_tokens.emplace_back( TokenKind::String,  m_partialToken.getValue(), m_partialToken.getLocation() );
                        if (onlyHeader) headerTokens++;
                        clearPartialToken();
                    } 
                    else 
                    {
                        appendPartialToken( value, m_lineNum, pos + match.length() );
                        if ( m_partialToken.getType() == TokenKind::Comment && value.ends_with("*/") ) 
                        {
                            clearPartialToken();
                        }
                    }

                    pos += match.length();
                    continue;
                }

                if ( !value.empty() && value.front() == '"' ) 
                {
                    setPartialToken( TokenKind::String, value, m_lineNum, pos );
                    pos += match.length();
                    continue;
                }

                if ( !value.empty() && value.starts_with( "/*" ) ) 
                {
                    setPartialToken( TokenKind::Comment, value, m_lineNum, pos );
                    pos += match.length();
                    continue;
                }

                SourceRange range = { 
                    { m_lineNum, pos }, 
                    { m_lineNum, pos + match.length() },
                    fileId
                };

                TokenKind type = getTokenType( value );

                if ( type == TokenKind::Keyword ) 
                {
                    TokenKeyword kw = m_KEYWORDS.at( value );
                    m_tokens.emplace_back( type, kw, value, range );
                }
                else if ( type == TokenKind::Symbol )
                {
                    TokenSymbol symbol = m_SYMBOLS.at( value );
                    m_tokens.emplace_back( type, symbol, value, range );
                }
                else
                {
                    m_tokens.emplace_back( type, value, range );
                }

                if ( onlyHeader ) headerTokens++;

                pos += match.length();
            } 
            else 
            {
                if ( !( m_inToken && m_partialToken.getType() == TokenKind::Comment ) ) 
                {
                    SourceRange errorLocation = {
                        { m_lineNum, pos },
                        { m_lineNum, pos + match.length() },
                        fileId
                    };

                    m_errReporter.report( CompilerError( 
                            ErrorSeverity::Error,
                            "Unexpected token at line " + std::to_string( m_lineNum ) + 
                            " position " + std::to_string( pos ) + ": '" + line[ pos ] + "'",
                            errorLocation,
                            ErrorCategory::Lexical
                        ) 
                    );   
                }

                pos++;
            }
        }
    }

    SourceRange eofLocation = {
        { m_lineNum, pos },
        { m_lineNum, pos },
        fileId
    };

    m_tokens.emplace_back( TokenKind::EndOfFile, "", eofLocation );

    checkIssueWithOutput(fileId);

    return m_tokens;
}


void Tokenizer::checkIssueWithOutput( size_t fileId ) 
{
    if ( !m_inToken ) return;

    size_t startLine = m_partialToken.getLocation().start.line;

    std::string lineStr = m_srcManager.getLine( fileId, startLine );
    size_t endColumn = lineStr.size();
    
    SourceRange errorLocation = {
        m_partialToken.getLocation().start,
        { startLine, endColumn },
        fileId
    };
    std::string errorMessage = "";

    if ( m_partialToken.getType() == TokenKind::String ) 
    {
        errorMessage = "Unterminated string literal at end of input";
    }
    else if ( m_partialToken.getType() == TokenKind::Comment ) 
    {
        errorMessage = "Unterminated multiline comment at end of input";
    }
    else
    {
        return;
    }

    m_errReporter.report( CompilerError( 
            ErrorSeverity::Fatal,
            errorMessage,
            errorLocation,
            ErrorCategory::Lexical
        ) 
    );
}

void Tokenizer::setPartialToken( TokenKind type, const std::string& value, size_t start_line, size_t start_pos ) 
{
    m_inToken = true;
    m_partialToken.setType( type );
    m_partialToken.addToValue( value );
    m_partialToken.setLocationStart( 
        { start_line, start_pos}
    );
}

void Tokenizer::appendPartialToken( const std::string& amendment, size_t line, size_t pos ) 
{
    m_partialToken.addToValue( amendment );
    m_partialToken.setLocationEnd(
        { line, pos }
    );
}

void Tokenizer::clearPartialToken() 
{
    m_inToken = false;
    m_partialToken.clear();
}

bool Tokenizer::matchRegex( const std::string& input, size_t pos, std::smatch& match, const std::regex& tokenPattern ) 
{
    return std::regex_search( input.begin() + pos, input.end(), match, tokenPattern ) && match.position() == 0;
}

TokenKind Tokenizer::getTokenType( const std::string& value ) 
{
    if ( std::regex_match( value, s_RE_INTEGER ) )           return TokenKind::Integer;
    else if ( std::regex_match( value, s_RE_FLOAT ) )        return TokenKind::Float;
    else if ( m_BOOLEANS.find( value ) != m_BOOLEANS.end() ) return TokenKind::Boolean;
    else if ( std::regex_match( value, s_RE_CHAR ) )         return TokenKind::Char;    
    else if ( m_KEYWORDS.find( value ) != m_KEYWORDS.end() ) return TokenKind::Keyword;
    else if ( m_SYMBOLS.find( value ) != m_SYMBOLS.end() )   return TokenKind::Symbol;
    
    return TokenKind::Identifier;
}

