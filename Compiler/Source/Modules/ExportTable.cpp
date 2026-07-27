/* === Main Import === */

#include "Modules/ExportTable.hpp"

/* === Export Table Methods === */

bool ExportTable::add( std::string_view name, SymbolId id )
{

    auto [it, inserted] = symbols.emplace( name, id );

    if (!inserted) return false;

    return true;
}

SymbolId ExportTable::find( std::string_view name )
{
    auto it = symbols.find( name );

    if ( it != symbols.end() ) 
    {   
        return it->second;
    }

    return SymbolId{};
}