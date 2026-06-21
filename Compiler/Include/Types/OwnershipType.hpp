#pragma once

/* === Imports === */

#include <memory>
#include "Utils/OwnershipKind.hpp"
#include "Types/Type.hpp"

/* === OwnershipTypeInfo === */

struct OwnershipType : Type 
{
    std::shared_ptr<Type> inner;
    OwnershipKind ownership;

    OwnershipType( TypeOrigin origin, std::shared_ptr<Type>&& inner, OwnershipKind owner ) 
        : Type( TypeKind::Ownership, origin ), inner( std::move( inner ) ), ownership( owner ) {}
};