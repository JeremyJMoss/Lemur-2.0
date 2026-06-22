#include "Tokens/Tokenizer.hpp"
#include "Tokens/Token.hpp"
#include "Driver/CompilationUnit.hpp"

using FileId = std::size_t;

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
    m_lineNum = 0;
    m_partialToken.clear();
    m_inToken = false;
}

std::unique_ptr<CompilationUnit> Tokenizer::tokenizeFile( const std::string& filePath ) 
{
    resetState();
    std::ifstream fileStream( filePath );

    if ( !fileStream.is_open() ) 
    {
        m_errReporter.report( RuntimeError(
                ErrorSeverity::Fatal,
                "Error opening .lmur file"
            ) 
        );
        return nullptr;
    }

    FileId fileId = m_srcManager.addFile(filePath);
    auto compUnit = std::make_unique<CompilationUnit>( fileId );
    
    tokenizeStream( fileStream, compUnit );

    return compUnit;
}

void Tokenizer::tokenizeStream( std::istream& stream, std::unique_ptr<CompilationUnit>& compUnit, bool onlyHeader ) 
{
    std::string line;
    std::size_t pos;
    std::size_t maxTokens = 20;
    std::size_t headerTokens = 0;

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
                        auto newToken = Token( TokenKind::String,  m_partialToken.getValue(), m_partialToken.getLocation() );
                        compUnit->addToken(newToken);
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
                    compUnit->getFileId()
                };

                TokenKind type = getTokenType( value );

                if ( type == TokenKind::Keyword ) 
                {
                    TokenKeyword kw = m_KEYWORDS.at( value );
                    auto newToken = Token( type, kw, value, range );
                    compUnit->addToken(newToken);
                }
                else if ( type == TokenKind::Symbol )
                {
                    TokenSymbol symbol = m_SYMBOLS.at( value );
                    auto newToken = Token( type, symbol, value, range );
                    compUnit->addToken(newToken);
                }
                else
                {
                    auto newToken = Token( type, value, range );
                    compUnit->addToken(newToken);
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
                        compUnit->getFileId()
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
        compUnit->getFileId()
    };

    auto newToken = Token( TokenKind::EndOfFile, "", eofLocation );
    compUnit->addToken(newToken);

    checkIssueWithOutput(compUnit->getFileId());
}


void Tokenizer::checkIssueWithOutput( FileId fileId ) 
{
    if ( !m_inToken ) return;

    std::size_t startLine = m_partialToken.getLocation().start.line;

    std::string lineStr = m_srcManager.getLine( fileId, startLine );
    std::size_t endColumn = lineStr.size();
    
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

void Tokenizer::setPartialToken( TokenKind type, const std::string& value, std::size_t start_line, std::size_t start_pos ) 
{
    m_inToken = true;
    m_partialToken.setType( type );
    m_partialToken.addToValue( value );
    m_partialToken.setLocationStart( 
        { start_line, start_pos}
    );
}

void Tokenizer::appendPartialToken( const std::string& amendment, std::size_t line, std::size_t pos ) 
{
    m_partialToken.addToValue( amendment );
    m_partialToken.setLocationEnd(
        { line, pos }
    );
}

bool Tokenizer::matchRegex( const std::string& input, std::size_t pos, std::smatch& match, const std::regex& tokenPattern ) 
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

