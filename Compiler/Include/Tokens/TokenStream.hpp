#pragma once

#include <span>
#include <expected>
#include "Tokens/Token.hpp"
#include "Errors/Errors.hpp"

class TokenStream {
public:
    TokenStream()
        : m_tokens({}), m_pos(0) {}

    Token peek( const std::size_t offset = 0 ) const;
    Token consume();
    void initializeTokenStream( std::span<const Token> tokens ) { m_tokens = tokens; }

    template <typename ExpectedValue>
    std::expected<Token, ErrorVariant> expect( TokenKind expectedType, ExpectedValue expectedValue );
    
    void reset();

private:
    std::span<const Token> m_tokens;
    size_t m_pos;
};

template <typename ExpectedValue>
std::expected<Token, ErrorVariant> TokenStream::expect( TokenKind expectedType, ExpectedValue expectedValue )
{
    Logger::trace(
        "Expecting token",
        std::to_array<Attribute>({
            { "ExpectedType", "'" + toString( expectedType ) + "'" },
            { "ExpectedValue", "'" + toString( expectedValue ) + "'" }
        })
    );

    auto maybeToken = peek();

    if ( !maybeToken ) 
    {
        Logger::trace( "Unexpected end of input" );
        return std::unexpected( maybeToken.error() );
    }

    auto nextToken = maybeToken.value();

    if ( !nextToken.checkTypeMatches( expectedType ) ) {
        Logger::trace(
            "Expect mismatch",
            std::to_array<Attribute>({
                { "ExpectedValue", "'" + toString( expectedValue ) + "'" },
                { "Value", "'" + nextToken.getValue() + "'" },
                { "Type", "'" + toString( nextToken.getType() ) + "'" }
            })
        );

        return std::unexpected(
            CompilerError(
                ErrorSeverity::Error,
                "Expected '" + toString( expectedValue ) + "', got '" + nextToken.getValue() + "'",
                nextToken.getLocation(),
                ErrorCategory::Syntax
            )
        );
    }

    if ( !nextToken.checkValueMatches( expectedValue ) ) {

        Logger::trace(
            "Expect mismatch. Token type matched but value mismatch. Rolling back position.",
            std::to_array<Attribute>({ 
                { "ExpectedValue", "'" + toString( expectedValue ) + "'" },
                { "Value", "'" + nextToken.getValue() + "'" }
            })
        );

        return std::unexpected(
            CompilerError(
                ErrorSeverity::Error,
                "Expected '" + toString( expectedValue ) + "', got '" + nextToken.getValue() + "'",
                nextToken.getLocation(),
                ErrorCategory::Syntax
            )
        );
    }

    auto token = consume();

    Logger::trace(
        "Expect succeeded",
        std::to_array<Attribute>({
            { "Type", "'" + toString( token.getType() ) + "'" },
            { "Value", "'" + token.getValue() + "'" }
        })
    );

    return token;
}