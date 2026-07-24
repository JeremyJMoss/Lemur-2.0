#pragma once

/* === Dependencies === */

#include <unordered_map>
#include <string>
#include <variant>

/* === Imports === */

#include "Core/Ids.hpp"
#include "Scopes/ScopeOwnerKind.hpp"

using NameBinding = std::variant<SymbolId, OverloadSetId>;

/* === Scope === */

struct Scope {
    ScopeId m_id;
    ScopeId m_parentId;
    ScopeOwnerKind m_kind;
    std::unordered_map<std::string, NameBinding> m_declarations;

    bool hasParent() const;

    bool insert( std::string_view name, NameBinding nameBinding );

    bool remove( std::string_view name );

    Scope( ScopeId id, ScopeId parentId, ScopeOwnerKind kind )
        : m_id( id ), 
          m_parentId( parentId ), 
          m_kind( kind ) {}
};