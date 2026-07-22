#pragma once

/* === Dependencies ===*/

#include <vector>
#include <span>

/* === Imports === */

#include "Tokens/Token.hpp"
#include "Core/Ids.hpp"

/* === Token Table === */

class TokenTable
{
    public:
        TokenId add( Token token );

        Token& get( TokenId id );

        const Token& get( TokenId id ) const;

        size_t count() const;

        std::span<const Token> getReadOnlyTokens() const;

        TokenTable() {}

    private:
        std::vector<Token> m_tokens;
};