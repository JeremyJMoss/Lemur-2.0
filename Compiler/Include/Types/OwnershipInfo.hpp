#pragma once

/* === Imports === */

#include <memory>
#include "Utils/OwnershipKind.hpp"

/* === OwnershipTypeInfo === */

using TypeId = std::size_t;

struct OwnershipInfo
{
    TypeId m_inner;
    OwnershipKind m_ownership;

    OwnershipInfo( TypeId inner, OwnershipKind owner ) 
        : m_inner( inner ), m_ownership( owner ) {}
};