/* === Main Import === */

#include "Core/CompilerContext.hpp"
#include "BuiltInRegistry/BuiltInRegistry.hpp"

/* === Compiler Context Methods === */

void CompilerContext::initialize()
{
    m_builtinScope = m_scopes.add(
        InvalidScopeId,
        ScopeOwnerKind::BuiltIn
    );

    BuiltInRegistry::initialize(*this);
}

std::expected<void, Diagnostic> CompilerContext::declareInScope( ScopeId scopeId, std::string_view name, SymbolId symbolId )
{
    auto result = m_scopes.declare( scopeId, name, symbolId );

    if (!result)
    {
        SymbolId previous = result.error();

        const Symbol& oldSymbol = m_symbols.get(previous);

        return std::unexpected(
            Diagnostic(
                std::format(
                    "Symbol '{}' already declared in current scope. Cannot redeclare symbol '{}'",
                    name, 
                    name
                ),
                ErrorCategory::Semantic,
                ErrorSeverity::Error,
                oldSymbol.declaration
            )
        );
    }

    return {};
}

ScopeId CompilerContext::enterScope(NodeId owner, ScopeOwnerKind kind)
{
    if (owner != InvalidNodeId) {
        NodeSemanticInfo& info = m_semanticInfo.getOrCreate( owner );

        if ( info.scope != InvalidScopeId )
        {
            m_scopeStack.enter( info.scope );
            return info.scope;
        }
    }

    auto scope = m_scopes.add( m_scopeStack.current(), kind );

    if (owner != InvalidNodeId) {
        m_semanticInfo.bindScope( owner, scope );
    }

    m_scopeStack.enter(scope);
    return scope;
}

void CompilerContext::leaveScope()
{
    m_scopeStack.leave();
}

ScopeId CompilerContext::currentScope() const
{
    return m_scopeStack.current();
}