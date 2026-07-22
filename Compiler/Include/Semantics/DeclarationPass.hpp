#pragma once

/* === Declarations === */

#include <vector>

/* === Imports === */

#include "Core/Ids.hpp"
#include "Scopes/ScopeStack.hpp"

/* === Forward Declarations === */

class CompilationUnit;

/* === Declaration Pass === */

class DeclarationPass
{
    public:
        void run( CompilationUnit& compUnit );

    private:
        ScopeId m_currentScope;
        ScopeStack m_scopeStack;
};