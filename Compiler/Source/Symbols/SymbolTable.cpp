/* === Dependencies ===*/

#include "Symbols/SymbolTable.hpp"

/* === Symbol Table Methods === */

SymbolId SymbolTable::add( Symbol* symbol ) 
{
    SymbolId id{ symbols.size() };

    symbol->setId( id );

    symbols.push_back( std::move( symbol ) );

    return id;
}

Symbol& SymbolTable::get( SymbolId id ) 
{
    return *symbols[id.value];
}

const Symbol& SymbolTable::get( SymbolId id ) const 
{
    return *symbols[id.value];
}

std::span<const Symbol* const> SymbolTable::getAll() const 
{
    return symbols;
}

