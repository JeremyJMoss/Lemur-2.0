#pragma once

/* === Dependencies === */

#include <optional>

/* === Imports === */

#include "Core/Ids.hpp"

/* === Array Info === */

struct ArrayInfo {
    TypeId elementType;
    std::optional<std::size_t> capacity; // null = dynamic from size of array value
};