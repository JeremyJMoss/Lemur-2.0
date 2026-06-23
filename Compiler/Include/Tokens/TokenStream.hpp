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
        void recoverFromError(); 

        std::expected<Token, ErrorVariant> expect( TokenKind expectedType, TokenSymbol expectedValue );
        std::expected<Token, ErrorVariant> expect( TokenKind expectedType, TokenKeyword expectedValue );
        
        void reset();

    private:
        std::span<const Token> m_tokens;
        size_t m_pos;
};