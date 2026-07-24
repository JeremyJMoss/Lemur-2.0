/* === Main Import === */

#include "Scopes/ScopeTable.hpp"

/* === Scope Table Methods === */

ScopeId ScopeTable::add( ScopeId parentId, ScopeOwnerKind kind ) 
{
    ScopeId id{ m_scopes.size() };

    m_scopes.emplace_back( id, parentId, kind );

    return id;
}

Scope* ScopeTable::get( ScopeId scopeId ) 
{
    if ( scopeId.value >= m_scopes.size() ) throw InternalCompilerError( "Attempted to get scope outside of scope table bounds.\nPlease report this bug." );

    return &m_scopes.at( scopeId.value ); 
}

bool ScopeTable::insert( ScopeId scopeId, std::string_view name, NameBinding nameBinding )
{
    Scope* scope = get( scopeId );

    if (!scope->insert( name, nameBinding )) throw InternalCompilerError( "Attempt to insert symbol into scope failed.\n Please report this bug." );

    return {};
}

std::optional<NameBinding> ScopeTable::lookup( ScopeId scopeId, std::string_view name )
{
    if ( !scopeId.valid() ) throw InternalCompilerError( "Lookup function called with InvalidScopeId.\nPlease report this bug." );

    while ( !scopeId.valid() )
    {
        Scope* scope = get( scopeId );

        auto it = scope->m_declarations.find( std::string(name) );

        if ( it != scope->m_declarations.end()) return it->second;

        scopeId = scope->m_parentId;
    }

    return std::nullopt;
}

std::optional<NameBinding> ScopeTable::lookupLocal( ScopeId scopeId, std::string_view name )
{
    if ( !scopeId.valid() ) throw InternalCompilerError( "Lookup function called with InvalidScopeId.\nPlease report this bug.");

    Scope* scope = get( scopeId );

    auto it = scope->m_declarations.find( std::string(name) );

    if ( it != scope->m_declarations.end() ) return it->second;

    return std::nullopt;
}