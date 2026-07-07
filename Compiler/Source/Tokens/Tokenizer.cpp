#include "Tokens/Tokenizer.hpp"
#include "Tokens/Token.hpp"
#include "Driver/CompilationUnit.hpp"
#include "Errors/ErrorReporter.hpp"
#include <cctype>
#include <expected>
#include <iostream>

using FileId = std::size_t;

bool Tokenizer::isWhitespaceChar( char c ) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f';
}

bool Tokenizer::isDigitChar( char c ) {
    return c >= '0' && c <= '9';
}

bool Tokenizer::isIdentifierStartChar( char c ) {
    return std::isalpha( static_cast<unsigned char>( c ) ) || c == '_';
}

bool Tokenizer::isIdentifierPartChar( char c ) {
    return std::isalnum( static_cast<unsigned char>( c ) ) || c == '_';
}

std::string_view Tokenizer::readIdentifier( std::string_view line, std::size_t& pos ) {
    std::size_t start = pos;
    while ( pos < line.size() && isIdentifierPartChar( line[ pos ] ) ) {
        pos++;
    }
    return line.substr( start, pos - start );
}

std::string_view Tokenizer::readNumber( std::string_view line, std::size_t& pos ) {
    std::size_t start = pos;
    while ( pos < line.size() && isDigitChar( line[ pos ] ) ) {
        pos++;
    }

    if ( pos + 1 < line.size() && line[ pos ] == '.' && isDigitChar( line[ pos + 1 ] ) ) {
        pos++;
        while ( pos < line.size() && isDigitChar( line[ pos ] ) ) {
            pos++;
        }
    }

    return line.substr( start, pos - start );
}

std::string_view Tokenizer::readSymbol( std::string_view line, std::size_t& pos, const std::unordered_map<std::string_view, TokenSymbol>& symbols ) {
    if ( pos + 1 < line.size() ) {
        std::string_view two( line.data() + pos, 2 );
        if ( symbols.find( two ) != symbols.end() ) {
            pos += 2;
            return two;
        }
    }

    std::string_view one( line.data() + pos, 1 );
    if ( symbols.find( one ) != symbols.end() ) {
        pos += 1;
        return one;
    }

    return "";
}

bool Tokenizer::isValidIdentifier( std::string_view s )
{
    if (s.empty())
        return false;

    if (!Tokenizer::isIdentifierStartChar(s[0]))
        return false;

    for (char c : s)
    {
        if (!Tokenizer::isIdentifierPartChar(c))
            return false;
    }

    return true;
}

std::expected<std::string, ConfigError> Tokenizer::parseModuleName( std::string_view input )
{
    std::string moduleName = "";

    std::size_t start = 0;

    while (start < input.size())
    {
        std::size_t end = input.find('.', start);

        if (end == std::string_view::npos)
            end = input.size();

        std::string_view part = input.substr(start, end - start);

        if (!isValidIdentifier(part))
        {
            return std::unexpected(
                ConfigError("Invalid module name: '" + std::string(input) + "'")
            );
        }

        moduleName += part;

        if (end == input.size())
            break;

        moduleName += '.';

        start = end + 1;
    }

    return moduleName;
}

std::expected<std::string, ModuleHeaderError> Tokenizer::readModuleHeader( std::istream& stream )
{
    std::string line;
    std::size_t pos = 0;

    enum class State
    {
        Start,
        ExpectModule,
        ExpectName,
        ExpectDotOrEnd
    };

    State state = State::Start;

    std::string moduleName = "";

    bool inComment = false;

    while (std::getline(stream, line))
    {
        pos = 0;

        while (pos < line.length())
        {
            char c = line[pos];

            // Handle block comments
            if ( inComment )
            {
                if (c == '*' && pos + 1 < line.length() && line[pos + 1] == '/')
                {
                    inComment = false;
                    pos += 2;
                }
                else
                {
                    ++pos;
                }
                continue;
            }

            // Skip whitespace
            if (isWhitespaceChar(c))
            {
                ++pos;
                continue;
            }

            // Skip Line comment
            if (c == '/' && pos + 1 < line.length() && line[pos + 1] == '/')
            {
                break;
            }

            // Enter multiline comment
            if (c == '/' && pos + 1 < line.length() && line[pos + 1] == '*')
            {
                inComment = true;
                pos += 2;
                continue;
            }

            // Identifier
            if (isIdentifierStartChar(c))
            {
                std::string_view id = readIdentifier( line, pos );

                if (state == State::Start)
                {
                    if (id != "module")
                        return std::unexpected(ModuleHeaderError("Invalid module header directive", ErrorSeverity::Error)); // invalid header

                    state = State::ExpectName;
                    continue;
                }

                if (state == State::ExpectName)
                {
                    moduleName += std::string( id );
                    state = State::ExpectDotOrEnd;
                    continue;
                }

                return ""; // invalid
            }

            // Dot (for module paths)
            if (c == '.')
            {
                if (state != State::ExpectDotOrEnd)
                    return std::unexpected(ModuleHeaderError("Malformed module header directive", ErrorSeverity::Error)); // invalid

                moduleName += '.';
                state = State::ExpectName;
                ++pos;
                continue;
            }

            // End of module header
            if (c == ';')
            {
                if (state != State::ExpectDotOrEnd)
                    return std::unexpected(ModuleHeaderError("Incomplete module header directive missing ending ';'", ErrorSeverity::Error)); // invalid incomplete module

                return moduleName;
            }

            // Anything else is invalid
            return std::unexpected(ModuleHeaderError("Invalid module header directive", ErrorSeverity::Error));
        }
    }

    return std::unexpected(ModuleHeaderError("No module directive found", ErrorSeverity::Warning)); // EOF without valid module header
}

void Tokenizer::tokenizeStream( std::istream& stream ) 
{
    std::string line;
    std::size_t pos;

    while ( true ) 
    {
        m_lineNum++;
        pos = 0;

        if ( !std::getline( stream, line ) ) break;

        m_lastLineLength = line.length();

        while ( pos < line.length() ) 
        {
            if ( m_inToken ) 
            {
                if ( m_partialToken.getType() == TokenKind::String ) 
                {
                    bool closed = false;

                    while ( pos < line.length() ) 
                    {
                        if ( line[ pos ] == '"' ) 
                        {
                            appendPartialToken( "\"", m_lineNum, pos + 1 );
                            auto newToken = Token( TokenKind::String, m_partialToken.getValue(), m_partialToken.getLocation() );
                            m_compUnit.addToken( newToken );
                            clearPartialToken();
                            pos++;
                            closed = true;
                            break;
                        }
                        
                        if ( line[ pos ] == '\\' && pos + 1 < line.length() ) 
                        {
                            appendPartialToken( std::string_view( line ).substr( pos, 2 ), m_lineNum, pos + 2 );
                            pos += 2;
                        }
                        else 
                        {
                            appendPartialToken( std::string_view( line ).substr( pos, 1 ), m_lineNum, pos + 1 );
                            pos++;
                        }
                    }

                    if ( !closed ) break;
                    continue;
                }

                if ( m_partialToken.getType() == TokenKind::MultiLineComment ) 
                {
                    bool closed = false;

                    while ( pos < line.length() ) 
                    {
                        if ( line[ pos ] == '*' && pos + 1 < line.length() && line[ pos + 1 ] == '/' ) 
                        {
                            appendPartialToken( "*/", m_lineNum, pos + 2 );
                            clearPartialToken();
                            pos += 2;
                            closed = true;
                            break;
                        }

                        appendPartialToken( std::string_view( line ).substr( pos, 1 ), m_lineNum, pos + 1 );
                        pos++;
                    }

                    if ( !closed ) break;
                    continue;
                }
            }

            char c = line[ pos ];

            if ( isWhitespaceChar( c ) ) 
            {
                pos++;
                continue;
            }

            if ( c == '"' ) 
            {
                setPartialToken( TokenKind::String, "\"", m_lineNum, pos );
                pos++;
                continue;
            }

            if ( c == '/' && pos + 1 < line.length() && line[ pos + 1 ] == '*' ) 
            {
                setPartialToken( TokenKind::MultiLineComment, "/*", m_lineNum, pos );
                pos += 2;
                continue;
            }

            if ( c == '/' && pos + 1 < line.length() && line[ pos + 1 ] == '/' )
            {
                break;
            }

            if ( c == '\'' ) 
            {
                std::size_t start = pos;
                pos++;

                if ( pos < line.length() ) 
                {
                    if ( line[ pos ] == '\\' && pos + 1 < line.length() ) 
                    {
                        pos += 2;
                    }
                    else 
                    {
                        pos++;
                    }
                }

                if ( pos < line.length() && line[ pos ] == '\'' ) 
                {
                    pos++;
                }

                std::string_view value = std::string_view( line ).substr( start, pos - start );
                SourceRange range = {
                    { m_lineNum, start },
                    { m_lineNum, pos },
                    m_compUnit.getFileId()
                };

                auto newToken = Token( TokenKind::Char, std::string( value ), range );
                m_compUnit.addToken( newToken );
                continue;
            }

            if ( isDigitChar( c ) ) 
            {
                std::size_t start = pos;
                std::string_view value = readNumber( line, pos );

                TokenKind type = getTokenType( value );
                SourceRange range = {
                    { m_lineNum, start },
                    { m_lineNum, pos },
                    m_compUnit.getFileId()
                };

                auto newToken = Token( type, std::string( value ), range );
                m_compUnit.addToken( newToken );
                continue;
            }

            if ( isIdentifierStartChar( c ) ) 
            {
                std::size_t start = pos;
                std::string_view value = readIdentifier( line, pos );
                TokenKind type = getTokenType( value );
                SourceRange range = {
                    { m_lineNum, start },
                    { m_lineNum, pos },
                    m_compUnit.getFileId()
                };

                if ( type == TokenKind::Keyword ) 
                {
                    TokenKeyword kw = m_KEYWORDS.at( value );
                    auto newToken = Token( type, kw, std::string( value ), range );
                    m_compUnit.addToken( newToken );
                }
                else 
                {
                    auto newToken = Token( type, std::string( value ), range );
                    m_compUnit.addToken( newToken );
                }

                continue;
            }

            std::string_view symbol = readSymbol( line, pos, m_SYMBOLS );
            if ( !symbol.empty() ) 
            {
                SourceRange range = {
                    { m_lineNum, pos - symbol.size() },
                    { m_lineNum, pos },
                    m_compUnit.getFileId()
                };

                TokenSymbol symbolType = m_SYMBOLS.at( symbol );
                auto newToken = Token( TokenKind::Symbol, symbolType, std::string( symbol ), range );
                m_compUnit.addToken( newToken );
                continue;
            }

            SourceRange errorLocation = {
                { m_lineNum, pos },
                { m_lineNum, pos + 1 },
                m_compUnit.getFileId()
            };

            m_errReporter.report( CompilerError(
                "Unexpected token at line " + std::to_string( m_lineNum ) +
                " position " + std::to_string( pos ) + ": '" + std::string( 1, c ) + "'",
                ErrorSeverity::Error,
                errorLocation,
                ErrorCategory::Lexical
            ) );

            pos++;
        }
    }

    SourceRange eofLocation = {
        { m_lineNum, pos },
        { m_lineNum, pos },
        m_compUnit.getFileId()
    };

    auto newToken = Token( TokenKind::EndOfFile, "", eofLocation );
    m_compUnit.addToken( newToken );

    checkIssueWithOutput( m_compUnit.getFileId() );
}

void Tokenizer::checkIssueWithOutput( FileId fileId ) 
{
    if ( !m_inToken ) return;

    std::size_t startLine = m_partialToken.getLocation().start.line;
    
    SourceRange errorLocation = {
        m_partialToken.getLocation().start,
        { startLine, m_lastLineLength },
        fileId
    };
    
    std::string errorMessage = "";

    if ( m_partialToken.getType() == TokenKind::String ) 
    {
        errorMessage = "Unterminated string literal at end of input";
    }
    else if ( m_partialToken.getType() == TokenKind::MultiLineComment ) 
    {
        errorMessage = "Unterminated multiline comment at end of input";
    }
    else
    {
        return;
    }

    m_errReporter.report( CompilerError( 
            errorMessage,
            ErrorSeverity::Fatal,
            errorLocation,
            ErrorCategory::Lexical
        ) 
    );
}

void Tokenizer::setPartialToken( TokenKind type, std::string_view value, std::size_t start_line, std::size_t start_pos ) 
{
    m_inToken = true;
    m_partialToken.setType( type );
    m_partialToken.addToValue( value );
    m_partialToken.setLocationStart( 
        { start_line, start_pos }
    );
}

void Tokenizer::appendPartialToken( std::string_view amendment, std::size_t line, std::size_t pos ) 
{
    m_partialToken.addToValue( amendment );
    m_partialToken.setLocationEnd(
        { line, pos }
    );
}

TokenKind Tokenizer::getTokenType( std::string_view value ) 
{
    bool isInteger = !value.empty();
    bool isFloat = false;
    for ( std::size_t i = 0; i < value.size(); ++i ) {
        if ( value[ i ] == '.' ) {
            if ( isFloat ) {
                isInteger = false;
                break;
            }
            isFloat = true;
            if ( i == 0 || i + 1 == value.size() ) {
                isInteger = false;
                break;
            }
            continue;
        }
        if ( !isDigitChar( value[ i ] ) ) {
            isInteger = false;
            isFloat = false;
            break;
        }
    }

    if ( isInteger ) return TokenKind::Integer;
    if ( isFloat ) return TokenKind::Float;
    if ( m_BOOLEANS.find( value ) != m_BOOLEANS.end() ) return TokenKind::Boolean;
    if ( m_KEYWORDS.find( value ) != m_KEYWORDS.end() ) return TokenKind::Keyword;
    if ( m_SYMBOLS.find( value ) != m_SYMBOLS.end() ) return TokenKind::Symbol;
    return TokenKind::Identifier;
}

void Tokenizer::clearPartialToken() 
{
    m_inToken = false;
    m_partialToken.clear();
}

