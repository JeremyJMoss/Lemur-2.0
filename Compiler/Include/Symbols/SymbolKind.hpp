#pragma once

/* === Depndencies === */

#include <cstdint>
#include <string>

/* === Symbol Kind === */

enum class SymbolKind : uint8_t
{
    Variable,
    Function,
    Type
};

std::string_view toString( const SymbolKind& kind );