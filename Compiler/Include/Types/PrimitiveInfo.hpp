#pragma once

/* === Imports === */

#include <string>

/* === PrimitiveTypeInfo === */

struct PrimitiveInfo
{
    std::string name;

    PrimitiveInfo( std::string name ) 
        : name( std::move( name ) ) {}
};