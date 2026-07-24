#pragma once

/* === Dependencies ===*/

#include <vector>
#include <expected>
#include <string>
#include <variant>
#include <optional>

/* === Imports === */

#include "Scopes/Scope.hpp"
#include "Core/Ids.hpp"
#include "Errors/Errors.hpp"

using NameBinding = std::variant<SymbolId, OverloadSetId>;

/* === Scope Table === */

class ScopeTable {
    public:
        ScopeId add( ScopeId parentId, ScopeOwnerKind kind );

        Scope* get( ScopeId scopeId );

        bool insert( ScopeId scopeId, std::string_view name, NameBinding nameBinding );

        std::optional<NameBinding> lookup( ScopeId scopeId, std::string_view name );

        std::optional<NameBinding> lookupLocal( ScopeId scopeId, std::string_view name );

    private:
        std::vector<Scope> m_scopes;
};