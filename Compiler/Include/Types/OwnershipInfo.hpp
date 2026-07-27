#pragma once

/* === Imports === */

#include "Utils/OwnershipKind.hpp"
#include "Core/Ids.hpp"

/* === Ownership Type === */

struct OwnershipInfo
{
    const TypeId inner;
    const OwnershipKind ownership;

    OwnershipInfo( TypeId inner, OwnershipKind owner ) 
        : inner( inner ), 
          ownership( owner ) {}
};