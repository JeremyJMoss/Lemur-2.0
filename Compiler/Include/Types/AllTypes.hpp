#pragma once

/* === Imports === */

#include "Types/Type.hpp"
#include "Types/PrimitiveType.hpp"
#include "Types/FunctionType.hpp"
#include "Types/OwnershipType.hpp"
#include "Types/CustomType.hpp"
#include "Types/InferredType.hpp"
#include "Types/NullType.hpp"
#include "Types/UnresolvedType.hpp"
#include <string>
#include <sstream>

/* === Utility === */

inline const std::string toString( const Type& type ) 
{
    switch ( type.kind )
    {
        case TypeKind::Primitive: 
        {
            const auto* prim = dynamic_cast<const PrimitiveType*>( &type );
            return prim->name;
        }

        case TypeKind::Function: 
        {
            const auto* func = dynamic_cast<const FunctionType*>( &type );
            std::ostringstream oss;
            oss << "fn(";
            for ( std::size_t i = 0; i < func->paramTypes.size(); ++i ) 
            {
                if (i > 0) oss << ", ";
                oss << toString( *func->paramTypes[i] );
            }
            oss << ") -> " << toString( *func->returnType );
            return oss.str();
        }

        case TypeKind::Ownership: 
        {
            const auto* owner = dynamic_cast<const OwnershipType*>( &type );
            std::ostringstream oss;
            oss << toString( owner->ownership ) << " {";
            oss << toString( *owner->inner ) << "}";
            return oss.str();
        }

        case TypeKind::Custom: 
        {
            const auto* custom = dynamic_cast<const CustomType*>( &type );
            return custom->name;
        }

        case TypeKind::Inferred: 
        {
            return "infer";
        }

        case TypeKind::Null: 
        {
            return "null";
        }

        case TypeKind::Unresolved:
        {
            return "unresolved";
        }

        default:
        {
            return "unknown type";
        }
    }
}
