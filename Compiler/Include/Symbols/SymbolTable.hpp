#pragma once

/* === Dependencies === */

#include <vector>

/* === Imports === */

#include "Symbols/Symbol.hpp"
#include "Core/Ids.hpp"

/* === SymbolTable === */

class SymbolTable
{
    public:
        SymbolId add( Symbol symbol ) {
            SymbolId id = symbols.size();

            symbol.setId( id );

            symbols.push_back( std::move( symbol ) );

            return id;
        }

        Symbol& get( SymbolId id ) {
            return symbols[id];
        }

        const Symbol& get( SymbolId id ) const {
            return symbols[id];
        }

        SymbolTable() {}

    private:
        std::vector<Symbol> symbols;
};