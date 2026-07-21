#pragma once

/* === Dependencies ===*/

#include <string>

/* === Imports === */

#include "Types/BuiltInType.hpp"

/* === Primitive Type === */

struct PrimitiveInfo
{
    const PrimitiveType name;

    PrimitiveInfo( PrimitiveType primType ) 
        : name( primType ) {}
};