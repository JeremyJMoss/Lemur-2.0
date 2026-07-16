#pragma once

/* === Dependencies === */

#include <string>

/* === Attribute === */

struct Attribute {
    std::string_view key;
    std::string_view val;
};