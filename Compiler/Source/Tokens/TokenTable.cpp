/* === Main Import === */

#include "Tokens/TokenTable.hpp"

/* === Token Table Methods === */

TokenId TokenTable::add( Token token ) 
{
    TokenId id = token.getId();
    
    m_tokens.push_back( std::move( token ) );

    return id;
}

Token& TokenTable::get( TokenId id ) 
{
    return m_tokens[id.value];
}

const Token& TokenTable::get( TokenId id ) const 
{
    return m_tokens[id.value];
}

size_t TokenTable::count() const 
{ 
    return m_tokens.size(); 
}

std::span<const Token> TokenTable::getReadOnlyTokens() const 
{ 
    return m_tokens; 
}