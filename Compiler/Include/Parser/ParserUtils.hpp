#pragma once

/* === Imports === */

#include <expected>
#include <array>
#include "Tokens/Token.hpp"
#include "Errors/ErrorReporter.hpp"
#include "Utils/SourceLocation.hpp"

/* === Forward Declarations === */

class Parser;

/* === ParserUtils === */

class ParserUtils
{
    public:
        size_t getCurrentPosition() { return m_position; }

        const std::vector<Token>& getTokens() { return m_tokens; }

        std::expected<Token, ErrorVariant> peek( const size_t peekIndex = 1 );

        std::expected<Token, RuntimeError> peekBack( const size_t reviewIndex = 1 );

        std::expected<Token, ErrorVariant> consume( TokenKind expectedType );

        template <typename ExpectedValue>
        std::expected<Token, ErrorVariant> expect( TokenKind expectedType, ExpectedValue expectedValue );

        void recoverFromError();

        static SourceRange getLocation( const Token& startToken, const Token& endToken );

        static SourceRange getLocation( const Token& token );

        static size_t getPrecedence( TokenSymbol op );

        void reset( std::vector<Token>&& inputTokens ) { 
            m_tokens = inputTokens;
            m_position = 0;
        }

    private:
        size_t m_position;
        std::vector<Token> m_tokens;

        bool isTerminatingToken( const Token& token ) const;
        bool isEndBrace( const Token& token ) const;
        bool isFrontBrace( const Token& token ) const;
};

template <typename ExpectedValue>
std::expected<Token, ErrorVariant> ParserUtils::expect( TokenKind expectedType, ExpectedValue expectedValue )
{
    Logger::trace(
        "Expecting token",
        std::to_array<Attribute>({
            { "ExpectedType", "'" + toString( expectedType ) + "'" },
            { "ExpectedValue", "'" + toString( expectedValue ) + "'" }
        })
    );

    auto maybeToken = consume( expectedType );

    if ( !maybeToken ) 
    {
        Logger::trace( "Expect failed: consume() did not return a valid token" );

        auto maybeNextToken = peek();

        if ( !maybeNextToken ) 
        {
            Logger::trace( "Expect failed: even peek() gave no token (EOF)" );
            return std::unexpected( maybeNextToken.error() );
        }

        auto nextToken = maybeNextToken.value();
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

    auto token = maybeToken.value();
    if ( !token.checkValueMatches( expectedValue ) ) {

        Logger::trace(
            "Expect mismatch. Token type matched but value mismatch. Rolling back position.",
            std::to_array<Attribute>({ 
                { "ExpectedValue", "'" + toString( expectedValue ) + "'" },
                { "Value", "'" + token.getValue() + "'" }
            })
        );

        m_position--;
        return std::unexpected(
            CompilerError(
                ErrorSeverity::Error,
                "Expected '" + toString( expectedValue ) + "', got '" + token.getValue() + "'",
                token.getLocation(),
                ErrorCategory::Syntax
            )
        );
    }

    Logger::trace(
        "Expect succeeded",
        std::to_array<Attribute>({
            { "Type", "'" + toString( token.getType() ) + "'" },
            { "Value", "'" + token.getValue() + "'" }
        })
    );

    return token;
}