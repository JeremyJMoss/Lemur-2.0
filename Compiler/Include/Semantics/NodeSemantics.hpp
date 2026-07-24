#pragma once

/* === Dependencies === */

#include <unordered_map>

/* === Imports === */

#include "Core/Ids.hpp"

/* === Semantic Information === */

struct NodeSemanticInfo
{
    SymbolId symbol{};
    TypeId   type{};
    ScopeId  scope{};
};

/* === Node Semantics === */

class NodeSemantics
{
public:

    bool contains( NodeId id ) const;

    NodeSemanticInfo* find( NodeId id );

    const NodeSemanticInfo* find( NodeId id ) const;

    NodeSemanticInfo& getOrCreate( NodeId id );

    void bindSymbol( NodeId id, SymbolId symbol );

    void bindType( NodeId id, TypeId type );

    void bindScope( NodeId id, ScopeId scope );

private:

    std::unordered_map<NodeId, NodeSemanticInfo> m_info;
};