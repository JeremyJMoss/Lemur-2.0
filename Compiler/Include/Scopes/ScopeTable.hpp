#pragma once

/* === Dependencies ===*/

#include <vector>

/* === Imports === */

#include "Scopes/Scope.hpp"
#include "Core/Ids.hpp"

/* === Scope Table === */

class ScopeTable {
    public:
        ScopeId addScope(
            ScopeId parentId,
            ScopeOwnerKind kind
        ) {
            ScopeId id = m_scopes.size();

            m_scopes.emplace_back( id, parentId, kind );

            return id;
        }

    private:
        std::vector<Scope> m_scopes;
};