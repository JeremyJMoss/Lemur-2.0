#pragma once

/* === Dependencies === */

#include <cstdint>

/* === Built In Types === */

enum class PrimitiveType : uint8_t
{
    Int, // 64 bit
    Int32, // 32 bit
    Int16, // 16 bit
    Int8, // 8 bit

    UInt, // 64 bit unsigned
    UInt32, // 32 bit unsigned
    UInt16, // 16 bit unsigned
    UInt8, // 8 bit unsigned

    Float, // 64 bit 
    Float32, // 32 bit

    Bool,
    Char
};