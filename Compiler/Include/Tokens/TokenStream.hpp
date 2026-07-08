#pragma once

#include <span>
#include <expected>
#include <functional>
#include "Tokens/Token.hpp"
#include "Errors/Errors.hpp"

class TokenStream {
    public:
        TokenStream( std::span<const Token> tokens )
            : m_tokens( tokens ) {}

        const Token& peek( const std::size_t offset = 0 ) const;
        const Token& consume();
        void recoverFromError(); 

        std::expected<std::reference_wrapper<const Token>, Diagnostic> expect( TokenKind expectedType, TokenSymbol expectedValue );
        std::expected<std::reference_wrapper<const Token>, Diagnostic> expect( TokenKind expectedType, TokenKeyword expectedValue );

    private:
        std::span<const Token> m_tokens;
        size_t m_pos = 0;
};