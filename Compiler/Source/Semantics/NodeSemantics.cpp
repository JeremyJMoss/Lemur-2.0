/* === Main Imports === */

#include "Semantics/NodeSemantics.hpp"

/* === Node Semantics Methods === */

bool NodeSemantics::contains( NodeId id ) const
{
    return m_info.contains( id );
}

NodeSemanticInfo* NodeSemantics::find( NodeId id )
{
    auto it = m_info.find( id );

    if ( it == m_info.end() )
        return nullptr;

    return &it->second;
}

const NodeSemanticInfo* NodeSemantics::find( NodeId id ) const
{
    auto it = m_info.find( id );

    if ( it == m_info.end() )
        return nullptr;

    return &it->second;
}

NodeSemanticInfo& NodeSemantics::getOrCreate( NodeId id )
{
    return m_info[id];
}

void NodeSemantics::bindSymbol( NodeId id, SymbolId symbol )
{
    m_info[id].symbol = symbol;
}

void NodeSemantics::bindType( NodeId id, TypeId type )
{
    m_info[id].type = type;
}

void NodeSemantics::bindScope( NodeId id, ScopeId scope )
{
    m_info[id].scope = scope;
}