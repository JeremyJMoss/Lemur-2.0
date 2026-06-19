#pragma once

/* === Imports === */

#include <vector>
#include <memory>
#include "Types/TypeInfo.hpp"

/* === FunctionTypeInfo === */

struct FunctionTypeInfo : TypeInfo 
{
    std::vector<std::shared_ptr<TypeInfo>> paramTypes;
    std::shared_ptr<TypeInfo> returnType;

    FunctionTypeInfo( std::vector<std::shared_ptr<TypeInfo>>&& params, std::shared_ptr<TypeInfo>&& returnType ) 
        : TypeInfo( TypeKind::Function ), paramTypes( std::move( params ) ), returnType( std::move( returnType ) ) {}
};