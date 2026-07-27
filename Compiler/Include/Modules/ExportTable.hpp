#pragma once

/* === Dependencies === */

#include <unordered_map>
#include <string>

/* === Imports === */

#include "Core/Ids.hpp"

/* === Export Table === */

struct ExportTable
{
    std::unordered_map<std::string, SymbolId> symbols;
};