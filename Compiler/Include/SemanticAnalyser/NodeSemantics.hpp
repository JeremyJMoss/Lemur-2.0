#pragma once

#include <unordered_map>
#include "Core/Ids.hpp"

/* === Semantic Information === */

struct NodeSemanticInfo
{
    SymbolId symbol = InvalidSymbolId;
    TypeId   type   = InvalidTypeId;
    ScopeId  scope  = InvalidScopeId;
};

/* === Node Semantics === */

class NodeSemantics
{
public:

    bool contains(NodeId id) const
    {
        return m_info.contains(id);
    }

    NodeSemanticInfo* find(NodeId id)
    {
        auto it = m_info.find(id);

        if (it == m_info.end())
            return nullptr;

        return &it->second;
    }

    const NodeSemanticInfo* find(NodeId id) const
    {
        auto it = m_info.find(id);

        if (it == m_info.end())
            return nullptr;

        return &it->second;
    }

    NodeSemanticInfo& getOrCreate(NodeId id)
    {
        return m_info[id];
    }

    void bindSymbol(NodeId id, SymbolId symbol)
    {
        m_info[id].symbol = symbol;
    }

    void bindType(NodeId id, TypeId type)
    {
        m_info[id].type = type;
    }

    void bindScope(NodeId id, ScopeId scope)
    {
        m_info[id].scope = scope;
    }

private:

    std::unordered_map<NodeId, NodeSemanticInfo> m_info;
};