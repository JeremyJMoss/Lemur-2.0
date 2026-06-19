#pragma once

/* === Imports === */

#include <memory>
#include "Utils/OwnershipKind.hpp"
#include "Types/TypeInfo.hpp"

/* === OwnershipTypeInfo === */

struct OwnershipTypeInfo : TypeInfo 
{
    std::shared_ptr<TypeInfo> inner;
    OwnershipKind ownership;

    OwnershipTypeInfo( std::shared_ptr<TypeInfo>&& inner, OwnershipKind owner ) 
        : TypeInfo( TypeKind::Ownership ), inner( std::move( inner ) ), ownership( owner ) {}
};