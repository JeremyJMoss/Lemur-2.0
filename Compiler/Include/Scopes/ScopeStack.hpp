#pragma once

/* === Declarations === */

#include <vector>

/* === Imports === */

#include "Core/Ids.hpp"
#include "Scopes/ScopeOwnerKind.hpp"

/* === Forward Declarations === */

class CompilationUnit;

/* === Scope Stack === */

class ScopeStack {
    public:
        void enter( ScopeId scopeId );

        void leave();

        ScopeId current() const;

    private:
        std::vector<ScopeId> m_stack;
};