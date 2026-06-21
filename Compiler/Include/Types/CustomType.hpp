#pragma once

/* === Imports === */

#include <string>
#include <vector>
#include <memory>
#include "Types/Type.hpp"

/* === CustomType === */

struct CustomType : Type 
{
    std::string name;
    std::vector<std::shared_ptr<Type>> parameters;
    bool implementsIterator = false;
    std::shared_ptr<Type> elementType;

    CustomType( TypeOrigin origin, const std::string& name, std::vector<std::shared_ptr<Type>> args ) 
        : Type( TypeKind::Custom, origin ), name( std::move( name ) ), parameters( std::move( args ) ) {}
};