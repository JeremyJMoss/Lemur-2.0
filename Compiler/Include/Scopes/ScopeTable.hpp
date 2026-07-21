#pragma once

/* === Dependencies ===*/

#include <vector>
#include <expected>
#include <string>

/* === Imports === */

#include "Scopes/Scope.hpp"
#include "Core/Ids.hpp"
#include "Errors/Errors.hpp"

/* === Scope Table === */

class ScopeTable {
    public:
        ScopeId add( ScopeId parentId, ScopeOwnerKind kind );

        Scope* get( ScopeId scopeId );

        std::expected<void, SymbolId> declare( ScopeId scopeId, std::string_view name, SymbolId symbol );

        SymbolId lookup( ScopeId scopeId, std::string_view name );

        SymbolId lookupLocal( ScopeId scopeId, std::string_view name );

    private:
        std::vector<Scope> m_scopes;
};