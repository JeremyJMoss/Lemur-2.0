#pragma once

/* === Imports === */

#include <vector>
#include <memory>
#include "Types/Type.hpp"

/* === FunctionType === */

struct FunctionType : Type 
{
    std::vector<std::shared_ptr<Type>> paramTypes;
    std::shared_ptr<Type> returnType;

    FunctionType( TypeOrigin origin, std::vector<std::shared_ptr<Type>>&& params, std::shared_ptr<Type>&& returnType ) 
        : Type( TypeKind::Function, origin ), paramTypes( std::move( params ) ), returnType( std::move( returnType ) ) {}
};