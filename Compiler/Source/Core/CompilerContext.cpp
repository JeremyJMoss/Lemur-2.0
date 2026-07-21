/* === Main Import === */

#include "Core/CompilerContext.hpp"
#include "BuiltInRegistry/BuiltInRegistry.hpp"

/* === Compiler Context Methods === */

void CompilerContext::initialize()
{
    m_builtinScope = createScope(
        InvalidScopeId,
        ScopeOwnerKind::BuiltIn
    );

    BuiltInRegistry::initialize(*this);
}