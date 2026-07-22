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
        SymbolId add( Symbol symbol );

        Symbol& get( SymbolId id );

        const Symbol& get( SymbolId id ) const;

        SymbolTable() {}

    private:
        std::vector<Symbol> symbols;
};