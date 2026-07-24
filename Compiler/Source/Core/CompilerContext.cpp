/* === Main Import === */

#include "Core/CompilerContext.hpp"
#include "BuiltInRegistry/BuiltInRegistry.hpp"

/* === Compiler Context Methods === */

void CompilerContext::initialize()
{
    m_builtinScope = m_scopes.add(
        ScopeId{},
        ScopeOwnerKind::BuiltIn
    );

    m_scopeStack.enter( m_builtinScope );

    BuiltInRegistry::initialize( *this );
}

std::expected<void, Diagnostic> CompilerContext::declareInScope( ScopeId scopeId, std::string_view name, SymbolId symbolId )
{
    const Symbol& symbol = m_symbols.get(symbolId);

    auto existing = m_scopes.lookupLocal(scopeId, name);

    // No existing declaration with this name
    if ( !existing.has_value() )
    {
        if ( symbol.kind == SymbolKind::Function )
        {
            OverloadSet* overloadSet = m_globalArena.allocate<OverloadSet>();

            OverloadSetId overloadSetId = m_overloadSets.add( overloadSet );

            overloadSet->addOverload( symbolId );

            m_scopes.insert( scopeId, name, overloadSetId );

            return {};
        }

        // Normal symbol
        m_scopes.insert( scopeId, name, symbolId );

        return {};
    }


    // Existing declaration found
    NameBinding binding = existing.value();

    // Existing name is an overload set
    if ( std::holds_alternative<OverloadSetId>( binding ) )
    {
        OverloadSetId overloadSetId = std::get<OverloadSetId>(binding);

        // Only functions can join overload sets
        if ( symbol.kind != SymbolKind::Function )
        {
            return std::unexpected(
                Diagnostic(
                    std::format(
                        "Symbol '{}' conflicts with existing overload set.",
                        name
                    ),
                    ErrorCategory::Semantic,
                    ErrorSeverity::Error,
                    symbol.declaration
                )
            );
        }

        OverloadSet& overloadSet = m_overloadSets.get(overloadSetId);

        // Signature checking handled later
        overloadSet.addOverload( symbolId );

        return {};
    }


    // Existing name is a normal symbol
    SymbolId existingSymbolId = std::get<SymbolId>( binding );

    const Symbol& existingSymbol =  m_symbols.get( existingSymbolId );


    // Function cannot collide with normal symbol
    return std::unexpected(
        Diagnostic(
            std::format(
                "Symbol '{}' already declared in current scope.",
                name
            ),
            ErrorCategory::Semantic,
            ErrorSeverity::Error,
            existingSymbol.declaration
        )
    );
}

ScopeId CompilerContext::enterScope(NodeId owner, ScopeOwnerKind kind)
{
    if ( owner.valid() ) {
        NodeSemanticInfo& info = m_semanticInfo.getOrCreate( owner );

        if ( info.scope.valid() )
        {
            m_scopeStack.enter( info.scope );
            return info.scope;
        }
    }

    auto scope = m_scopes.add( m_scopeStack.current(), kind );

    if ( owner.valid() ) {
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