#include <vector>
#include <span>
#include "Tokens/Token.hpp"

using TokenId = std::size_t;

class TokenTable
{
    public:
        TokenId add( Token token ) {
            TokenId id = token.getId();
            
            m_tokens.push_back( std::move( token ) );

            return id;
        }

        Token& get( TokenId id ) {
            return m_tokens[id];
        }

        const Token& get( TokenId id ) const {
            return m_tokens[id];
        }

        size_t count() const { return m_tokens.size(); }

        std::span<const Token> getReadOnlyTokens() const { return m_tokens; }

        TokenTable() {}

    private:
        std::vector<Token> m_tokens;
};