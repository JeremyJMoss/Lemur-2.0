#pragma once

/* === Imports === */

#include <string>
#include "Types/TypeInfo.hpp"

/* === PrimitiveTypeInfo === */

struct PrimitiveTypeInfo : TypeInfo 
{
    std::string name;
    // size in bytes
    size_t size;
    // Alignment ensures the type is stored at a memory address that is a multiple 
    // of its alignment value. Required by many CPUs for efficient access.
    // Example: a 4-byte int has alignment 4, so it must start at an address divisible by 4. 
    // Used when computing struct padding, array strides, and stack layout.
    size_t alignment;

    PrimitiveTypeInfo( const std::string& name, size_t size, size_t alignment = 0 ) 
        : TypeInfo( TypeKind::Primitive ), name( std::move( name ) ), size( size ), alignment( alignment ) {}
};