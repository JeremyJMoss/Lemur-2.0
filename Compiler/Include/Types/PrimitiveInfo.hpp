#pragma once

/* === Imports === */

#include <string>

/* === Primitive Type === */

struct PrimitiveInfo
{
    const std::string name;

    PrimitiveInfo( std::string name ) 
        : name( std::move( name ) ) {}
};