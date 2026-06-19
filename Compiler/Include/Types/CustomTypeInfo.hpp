#pragma once

/* === Imports === */

#include <string>
#include <vector>
#include <memory>
#include "Types/TypeInfo.hpp"

/* === CustomTypeInfo === */

struct CustomTypeInfo : TypeInfo 
{
    std::string name;
    std::vector<std::shared_ptr<TypeInfo>> parameters;
    bool implementsIterator = false;
    std::shared_ptr<TypeInfo> elementType;

    CustomTypeInfo( const std::string& name, std::vector<std::shared_ptr<TypeInfo>> args ) 
        : TypeInfo( TypeKind::Custom ), name( std::move( name ) ), parameters( std::move( args ) ) {}
};