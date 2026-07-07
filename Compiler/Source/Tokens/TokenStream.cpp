#include "Tokens/TokenStream.hpp"
#include "Utils/Logger.hpp"

/**
 * Attempts to check the next token in the token list
 * 
 * @param const std::size_t peekIndex How far ahead to look
 * @return std::expected<Token, ErrorVariant> Token in the position from the current position in the token list specified 
 * by peekIndex returns Error otherwise if could not retrieve token
 */
const Token& TokenStream::peek( const std::size_t offset ) const
{
    size_t index = m_pos + offset;

    // clamp to EOF instead of failing
    if (index >= m_tokens.size())
        return m_tokens[m_tokens.size() - 1]; // EOF

    return m_tokens[index];
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
const Token& TokenStream::consume()
{
    if (m_pos >= m_tokens.size())
        return m_tokens[m_tokens.size() - 1];

    return m_tokens[m_pos++];
}

std::expected<std::reference_wrapper<const Token>, ErrorVariant> TokenStream::expect( TokenKind expectedType, TokenSymbol expectedValue )
{
    Logger::trace(
        "Expecting token",
        std::to_array<Attribute>({
            { "ExpectedType", "'" + toString( expectedType ) + "'" },
            { "ExpectedValue", "'" + toString( expectedValue ) + "'" }
        })
    );

    auto peekedToken = peek();

    if ( peekedToken.checkTypeMatches( TokenKind::EndOfFile ) ) 
    {
        return std::unexpected( UnexpectedEndOfInputError( peekedToken.getLocation() ) );
    }

    if ( !peekedToken.checkTypeMatches( expectedType ) ) {
        Logger::trace(
            "Expect mismatch",
            std::to_array<Attribute>({
                { "ExpectedType", "'" + toString( expectedType ) + "'" },
                { "Type", "'" + toString( peekedToken.getType() ) + "'" }
            })
        );

        return std::unexpected(
            UnexpectedTypeError( expectedType, peekedToken.getType(), peekedToken.getLocation() )
        );
    }

    if ( !peekedToken.checkValueMatches( expectedValue ) ) {

        Logger::trace(
            "Expect mismatch. Token type matched but value mismatch.",
            std::to_array<Attribute>({ 
                { "ExpectedValue", "'" + toString( expectedValue ) + "'" },
                { "Value", "'" + std::string( peekedToken.getValue() ) + "'" }
            })
        );

        return std::unexpected(
            UnexpectedValueError( expectedValue, peekedToken.getSymbol(), peekedToken.getLocation())
        );
    }

    const Token& token = consume();

    Logger::trace(
        "Expect succeeded",
        std::to_array<Attribute>({
            { "Type", "'" + toString( token.getType() ) + "'" },
            { "Value", "'" + std::string( token.getValue() ) + "'" }
        })
    );

    return std::cref(token);
}

std::expected<std::reference_wrapper<const Token>, ErrorVariant> TokenStream::expect( TokenKind expectedType, TokenKeyword expectedValue )
{
    Logger::trace(
        "Expecting token",
        std::to_array<Attribute>({
            { "ExpectedType", "'" + toString( expectedType ) + "'" },
            { "ExpectedValue", "'" + toString( expectedValue ) + "'" }
        })
    );

    auto peekedToken = peek();

    if ( peekedToken.checkTypeMatches( TokenKind::EndOfFile ) ) 
    {
        return std::unexpected( UnexpectedEndOfInputError( peekedToken.getLocation() ) );
    }

    if ( !peekedToken.checkTypeMatches( expectedType ) ) {
        Logger::trace(
            "Expect mismatch",
            std::to_array<Attribute>({
                { "ExpectedType", "'" + toString( expectedType ) + "'" },
                { "Type", "'" + toString( peekedToken.getType() ) + "'" }
            })
        );

        return std::unexpected(
            UnexpectedTypeError( expectedType, peekedToken.getType(), peekedToken.getLocation() )
        );
    }

    if ( !peekedToken.checkValueMatches( expectedValue ) ) {

        Logger::trace(
            "Expect mismatch. Token type matched but value mismatch.",
            std::to_array<Attribute>({ 
                { "ExpectedValue", "'" + toString( expectedValue ) + "'" },
                { "Value", "'" + std::string( peekedToken.getValue() ) + "'" }
            })
        );

        return std::unexpected(
            UnexpectedValueError( expectedValue, peekedToken.getKeyword(), peekedToken.getLocation() )
        );
    }

    const Token& token = consume();

    Logger::trace(
        "Expect succeeded",
        std::to_array<Attribute>({
            { "Type", "'" + toString( token.getType() ) + "'" },
            { "Value", "'" + std::string( token.getValue() ) + "'" }
        })
    );

    return std::cref(token);
}

void TokenStream::recoverFromError() 
{
    // setting up array for debugging purposes
    const std::array posAttr = {
        Attribute{ "Position", std::to_string( m_pos ) } 
    };

    Logger::trace( 
        "Error recovery started", 
        posAttr
    );

    int braceDepth = 0;

    while ( true ) 
    {
        auto token = peek();

        if ( token.checkTypeMatches( TokenKind::EndOfFile ) )
        {
            Logger::trace( 
                "Reached end of file", 
                posAttr
            );
            return; // EOF reached
        }
        else if ( token.checkMatches( TokenKind::Symbol, TokenSymbol::LBrace ) ) 
        {
            braceDepth++;
        } 
        else if ( token.checkMatches( TokenKind::Symbol, TokenSymbol::RBrace ) ) 
        {
            if ( braceDepth == 0 ) 
            {
                // Unmatched '}' -> probably a good sync point
                m_pos++;
                break;
            }
            braceDepth--;
        }

        // setting up array for debugging purposes
        const std::array attrs = {
            Attribute{ "Value", "'" + std::string( token.getValue() ) + "'" },
            Attribute{ "Position", std::to_string( m_pos ) },
            Attribute{ "Location", token.getLocation().toString() }
        };

        if ( braceDepth == 0 && token.checkMatches( TokenKind::Symbol, TokenSymbol::SemiColon ) ) 
        {
            Logger::trace( 
                "Sync token found", 
                attrs
            );
            m_pos++;
            break;
        }

        Logger::trace(
            "Skipping token", 
            attrs
        );

        m_pos++;
    }

    Logger::trace( 
        "Resumed parsing", 
        posAttr
    );
}