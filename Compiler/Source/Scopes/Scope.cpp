/* === Main Import === */

#include "Scopes/Scope.hpp"

/* === Scope Methods === */

bool Scope::hasParent() const 
{ 
    return m_parentId.valid(); 
}

bool Scope::insert( std::string_view name, NameBinding nameBinding )
{
    auto [it, success] = m_declarations.try_emplace( std::string( name ), nameBinding );

    return success;
}

bool Scope::remove( std::string_view name )
{
    return m_declarations.erase( std::string( name ) );
}