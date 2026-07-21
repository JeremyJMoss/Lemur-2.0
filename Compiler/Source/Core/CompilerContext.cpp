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