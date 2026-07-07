#pragma once

/* === Imports === */

#include <optional>

using ArraySize = std::size_t; 
using TypeId = std::size_t;

/* === Array Info === */

struct ArrayInfo {
    TypeId elementType;
    std::optional<ArraySize> capacity; // null = dynamic from size of array value
};