#include "Parser/ParserUtils.hpp"
#include "Utils/Logger.hpp"

/**
 * Attempts to check the next token in the token list
 * 
 * @param const std::size_t peekIndex How far ahead to look
 * @return std::expected<Token, ErrorVariant> Token in the position from the current position in the token list specified 
 * by peekIndex returns Error otherwise if could not retrieve token
 */
std::expected<Token, ErrorVariant> ParserUtils::peek( const std::size_t peekIndex )
{
    std::size_t index = m_position + peekIndex;

    if ( index >= m_tokens.size() ) 
    {
        Logger::trace( 
            "Peek out of bounds at index " + std::to_string( index ),
            std::to_array<Attribute>({
                { "TokensSize", std::to_string( m_tokens.size() ) }
            })
        );

        return std::unexpected(
            CompilerError(
                "Peek index out of bounds",
                ErrorSeverity::Error,
                {},
                ErrorCategory::Internal
            )
        );
    }

    auto nextToken = m_tokens.at( index );

    // Return next token in the list
    return nextToken;
}

/**
 * Attempts to check the most recently consumed token in the token list
 * 
 * @param const std::size_t reviewIndex How far behind to look
 * @return std::expected<Token, RuntimeError> in the position from the current position in the token list specified by the reviewIndex 
 * or Runtime Error if at first place in the list
 */
std::expected<Token, RuntimeError> ParserUtils::peekBack( const std::size_t reviewIndex ) 
{
    // Check if previous token is before token stream
    if (reviewIndex <= m_position)
    {
        return m_tokens.at(m_position - reviewIndex);
    }

    return std::unexpected( 
        RuntimeError( 
            ErrorSeverity::Fatal, 
            "Cannot view previous token: past token stream beginning" 
        ) 
    );
}

/**
 * Moves the pointer ahead to the next in the token list and returns that token if
 * the token matches the expectedType
 * 
 * @param const std::string& expectedType  Reference to the type that is to be consumed
 * @returns Token if matches expectedType
 * @throws std::runtime_error If reached end of input tokens
 * @throws ParseError If expected type does not match type of current token
 */
Token& ParserUtils::consume( TokenKind expectedType ) 
{
    auto maybeToken = peek();

    if ( !maybeToken ) 
    {
        Logger::trace( "Consume failed. No token available." );
        return std::unexpected( maybeToken.error() );
    }

    Token token = maybeToken.value();

    // if token m_ is not expected type then not valid statement
    if ( !token.checkTypeMatches( expectedType ) ) 
    {
        Logger::trace( 
            "Consume mismatch", 
            std::to_array<Attribute>({
                { "Expected", toString( expectedType ) },
                { "Received", toString( token.getType() ) }
            })
        );
                      
        return std::unexpected( 
            CompilerError(
                ErrorSeverity::Error,
                "Expected '" + toString( expectedType ) + "' got " + toString( token.getType() ), 
                token.getLocation(),
                ErrorCategory::Syntax 
            )
        );
    }

    Logger::trace( 
        "Consumed token", 
        std::to_array<Attribute>({ 
            { "Type", "'" + toString( token.getType() ) + "'" }, 
            { "Value", "'" + token.getValue() + "'" } 
        })
    );

    m_position++;

    return token;
}

bool ParserUtils::isTerminatingToken( const Token& token ) const 
{
    // treat semicolon or closing brace as sync points
    return token.checkMatches( TokenKind::Symbol, TokenSymbol::SemiColon );
}

bool ParserUtils::isEndBrace( const Token& token ) const
{
    return token.checkMatches( TokenKind::Symbol, TokenSymbol::RBrace );
}

bool ParserUtils::isFrontBrace( const Token& token ) const
{
    return token.checkMatches( TokenKind::Symbol, TokenSymbol::LBrace );
}

void ParserUtils::recoverFromError() 
{
    // setting up array for debugging purposes
    const std::array posAttr = {
        Attribute{ "Position", std::to_string( m_position ) } 
    };

    Logger::trace( 
        "Error recovery started", 
        posAttr
    );

    int braceDepth = 0;

    while ( true ) 
    {
        auto maybeToken = peek();
        if ( !maybeToken ) 
        {
            Logger::trace( 
                "Unexpected end of input", 
                posAttr
            );
            return; // Error
        }

        const Token& token = maybeToken.value();

        if ( token.checkTypeMatches(TokenKind::EndOfFile) )
        {
            Logger::trace( 
                "Reached end of file", 
                posAttr
            );
            return; // EOF reached
        }
        if ( isFrontBrace( token ) ) 
        {
            braceDepth++;
        } 
        else if ( isEndBrace( token ) ) 
        {
            if ( braceDepth == 0 ) 
            {
                // Unmatched '}' -> probably a good sync point
                m_position++;
                break;
            }
            braceDepth--;
        }

        // setting up array for debugging purposes
        const std::array attrs = {
            Attribute{ "Value", "'" + token.getValue() + "'" },
            Attribute{ "Position", std::to_string( m_position ) },
            Attribute{ "Location", token.getLocation().toString() }
        };

        if ( braceDepth == 0 && isTerminatingToken( token ) ) 
        {
            Logger::trace( 
                "Sync token found", 
                attrs
            );
            m_position++;
            break;
        }

        Logger::trace(
            "Skipping token", 
            attrs
        );

        m_position++;
    }

    Logger::trace( 
        "Resumed parsing", 
        posAttr
    );
}

/**
 * Gets the source range of the file based on the tokens passed in
 * 
 * @param const Token& startToken The start to the ast node tokens
 * @param const Token& endToken The end to the ast node tokens
 * @returns SourceRange the range from and to of the ast node
 */
SourceRange ParserUtils::getLocation( const Token& startToken, const Token& endToken ) 
{
    return { 
        startToken.getLocation().start, 
        endToken.getLocation().end,
        endToken.getLocation().fileId
    };
}

/**
 * Gets the source range of the file based on the tokens passed in
 * 
 * @param const Token& token The start and end to the ast node token
 * @returns SourceRange the range from and to of the ast node
 */
SourceRange ParserUtils::getLocation( const Token& token ) 
{
    return token.getLocation();
}

/**
 * Gets operator precedence
 * 
 * @param const std::string& op Operator to check precedence of
 * @returns The precedence number of the operator passed in 
 */
size_t ParserUtils::getPrecedence( TokenSymbol op ) 
{
    size_t prec = 0;

    switch (op){
        case TokenSymbol::Or:
            prec = 1;
            break;
        case TokenSymbol::And:
            prec = 2;
            break;
        case TokenSymbol::Equals:
        case TokenSymbol::NotEquals:
            prec = 3;
            break;
        case TokenSymbol::Less:
        case TokenSymbol::Greater:
        case TokenSymbol::LessEquals:
        case TokenSymbol::GreaterEquals:
            prec = 4;
            break;
        case TokenSymbol::Plus:
        case TokenSymbol::Minus:
            prec = 5;
            break;
        case TokenSymbol::Star:
        case TokenSymbol::Slash:
        case TokenSymbol::Percent:
            prec = 6;
            break;
    }

    Logger::trace( "Operator '" + toString( op ) + "' has precedence " + std::to_string( prec ) );
    return prec;
}