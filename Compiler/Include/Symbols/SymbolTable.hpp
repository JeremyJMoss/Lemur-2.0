#pragma once

/* === Imports === */
#include <vector>
#include "AST/ASTNode.hpp"
#include "Symbols/Symbol.hpp"
#include "Types/Type.hpp"
#include "SourceControl/SourceLocation.hpp"

/* === SymbolTable === */

using SymbolId = std::size_t;

class SymbolTable
{
    public:
        SymbolId add(Symbol symbol) {
            SymbolId id = symbols.size();

            symbol.setId( id );

            symbols.push_back(std::move(symbol));

            return id;
        }

        Symbol& get(SymbolId id) {
            return symbols[id];
        }

        const Symbol& get(SymbolId id) const {
            return symbols[id];
        }

        SymbolTable() {}

    private:
        std::vector<Symbol> symbols;
};