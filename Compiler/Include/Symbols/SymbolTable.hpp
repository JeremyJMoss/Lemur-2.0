#pragma once

/* === Dependencies === */

#include <vector>
#include <span>

/* === Imports === */

#include "Symbols/Symbol.hpp"
#include "Core/Ids.hpp"

/* === SymbolTable === */

class SymbolTable
{
    public:
        SymbolId add( Symbol* symbol );

        Symbol& get( SymbolId id );

        const Symbol& get( SymbolId id ) const;

        std::span<const Symbol* const> getAll() const;

        SymbolTable() {}

    private:
        std::vector<Symbol*> symbols;
};