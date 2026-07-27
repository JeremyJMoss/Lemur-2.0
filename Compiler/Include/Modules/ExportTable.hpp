#pragma once

/* === Dependencies === */

#include <unordered_map>
#include <string>

/* === Imports === */

#include "Core/Ids.hpp"

/* === Export Table === */

struct ExportTable
{
    bool add( std::string_view name, SymbolId id );

    SymbolId find( std::string_view name );

    std::unordered_map<std::string_view, SymbolId> symbols;
};