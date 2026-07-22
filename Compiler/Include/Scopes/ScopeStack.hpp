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
        ScopeId enter(NodeId owner, ScopeOwnerKind kind);

        void leave();

        ScopeId current() const;

    private:
        CompilationUnit& m_unit;
        std::vector<ScopeId> m_stack;
};