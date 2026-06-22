#pragma once

/* === Imports === */

#include <expected>
#include <array>
#include "Tokens/Token.hpp"
#include "Errors/ErrorReporter.hpp"
#include "SourceControl/SourceLocation.hpp"

/* === Forward Declarations === */

class Parser;

/* === ParserUtils === */

class ParserUtils
{
    public:
        std::size_t getCurrentPosition() { return m_position; }

        void recoverFromError();

        static SourceRange getLocation( const Token& startToken, const Token& endToken );

        static SourceRange getLocation( const Token& token );

        static std::size_t getPrecedence( TokenSymbol op );

        void reset( std::vector<Token>&& inputTokens ) { 
            m_tokens = inputTokens;
            m_position = 0;
        }

    private:
        std::size_t m_position;
        std::vector<Token> m_tokens;

        bool isTerminatingToken( const Token& token ) const;
        bool isEndBrace( const Token& token ) const;
        bool isFrontBrace( const Token& token ) const;
};