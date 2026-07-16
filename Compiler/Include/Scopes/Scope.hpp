#pragma once

/* === Imports === */

#include "Core/Ids.hpp"
#include "Scopes/ScopeOwnerKind.hpp"

/* === Scope === */

struct Scope {
    ScopeId m_id;
    ScopeId m_parentId;
    ScopeOwnerKind m_kind;

    bool hasParent( const Scope& scope ) { return scope.m_parentId != InvalidScopeId; }

    Scope( ScopeId id, ScopeId parentId, ScopeOwnerKind kind )
        : m_id( id ), 
          m_parentId( parentId ), 
          m_kind( kind ) {}
};