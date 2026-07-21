/* === Main Import === */

#include "Scopes/ScopeTable.hpp"

/* === Scope Table Methods === */

ScopeId ScopeTable::addScope(
    ScopeId parentId,
    ScopeOwnerKind kind
) {
    ScopeId id = m_scopes.size();

    m_scopes.emplace_back( id, parentId, kind );

    return id;
}

Scope* ScopeTable::getScope( ScopeId scopeId ) {
    if ( m_scopes.size() < scopeId - 1 )
    {
        throw new InternalCompilerError( "Attempted to get scope outside of scope table bounds.\nPlease report this bug." );
    }

    return &m_scopes.at( scopeId ); 
}

std::expected<void, SymbolId> ScopeTable::declare( ScopeId scopeId, std::string_view name, SymbolId symbolId )
{
    SymbolId existing = lookupLocal(scopeId, name);

    if ( existing != InvalidSymbolId )
    {
        return std::unexpected( existing );
    }

    Scope* scope = getScope( scopeId );

    if (!scope->insert( name, symbolId ))
    {
        throw InternalCompilerError("Attempt to insert symbol into scope failed.\n Please report this bug.");
    }

    return {};
}

SymbolId ScopeTable::lookup( ScopeId scopeId, std::string_view name )
{
    if ( scopeId == InvalidScopeId )
    {
        throw InternalCompilerError( "Lookup function called with InvalidScopeId.\nPlease report this bug.");
    }

    while ( scopeId != InvalidScopeId )
    {
        Scope* scope = getScope( scopeId );

        auto it = scope->m_declarations.find( std::string(name) );

        if ( it != scope->m_declarations.end())
        {
            return it->second;
        }

        scopeId = scope->m_parentId;
    }

    return InvalidSymbolId;
}

SymbolId ScopeTable::lookupLocal( ScopeId scopeId, std::string_view name )
{
    if ( scopeId == InvalidScopeId )
    {
        throw InternalCompilerError( "Lookup function called with InvalidScopeId.\nPlease report this bug.");
    }

    Scope* scope = getScope( scopeId );

    auto it = scope->m_declarations.find( std::string(name) );

    if ( it != scope->m_declarations.end() )
    {
        return it->second;
    }

    return InvalidSymbolId;
}