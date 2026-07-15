#pragma once

/* === Dependencies ===*/

#include "Core/Ids.hpp"

using ScopeId = size_t;
constexpr ScopeId InvalidScopeId = static_cast<ScopeId>(-1);

/* === Enum Declarations === */

enum class ScopeOwnerKind 
{
    Global,
    Function,
    Lambda,
    Block
};

/* === Scope === */

struct Scope {
    ScopeId m_id;
    ScopeId m_parentId;
    ScopeOwnerKind m_kind;

    bool hasParent( const Scope& scope ) {
        return scope.m_parentId != InvalidScopeId;
    }

    Scope( ScopeId id, ScopeId parentId, ScopeOwnerKind kind )
        : m_id( id ), m_parentId( parentId ), m_kind( kind ) {}
};