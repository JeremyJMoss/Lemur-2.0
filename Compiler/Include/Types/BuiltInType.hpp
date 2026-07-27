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

inline std::string toString ( const PrimitiveType prim )
{
    switch( prim )
    {
        case PrimitiveType::Int:     return "int";
        case PrimitiveType::Int32:   return "int32";
        case PrimitiveType::Int16:   return "int16";
        case PrimitiveType::Int8:    return "int8";
        case PrimitiveType::UInt:    return "uint";
        case PrimitiveType::UInt32:  return "uint32";
        case PrimitiveType::UInt16:  return "uint16";
        case PrimitiveType::UInt8:   return "uint8";
        case PrimitiveType::Float:   return "float";
        case PrimitiveType::Float32: return "float32";
        case PrimitiveType::Bool:    return "bool";
        case PrimitiveType::Char:    return "char";
        default:                     return "Unknown";
    }
}