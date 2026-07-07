#pragma once

/* === Imports === */

#include "Utils/OwnershipKind.hpp"

using TypeId = std::size_t;

/* === Ownership Type === */

struct OwnershipInfo
{
    const TypeId m_inner;
    const OwnershipKind m_ownership;

    OwnershipInfo( TypeId inner, OwnershipKind owner ) 
        : m_inner( inner ), 
          m_ownership( owner ) {}
};