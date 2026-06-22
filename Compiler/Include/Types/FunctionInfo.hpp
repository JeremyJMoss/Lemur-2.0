#pragma once

/* === Imports === */

#include <vector>
#include <memory>

/* === FunctionType === */

using TypeId = std::size_t;

struct FunctionInfo
{
    std::vector<TypeId> paramTypes;
    TypeId returnType;

    FunctionInfo( std::vector<TypeId>&& params, TypeId returnType ) 
        :  paramTypes( std::move( params ) ), returnType( returnType ) {}
};