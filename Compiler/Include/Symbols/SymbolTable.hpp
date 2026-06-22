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
            symbol.setId( symbols.size() );

            symbols.push_back(std::move(symbol));

            return symbol.m_id;
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