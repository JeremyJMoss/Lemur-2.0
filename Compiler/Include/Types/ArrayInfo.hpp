#pragma once
#include <optional>

using ArraySize = std::size_t; 
using TypeId = std::size_t;

struct ArrayInfo {
    TypeId elementType;
    std::optional<ArraySize> capacity; // null = dynamic from size of array value
};