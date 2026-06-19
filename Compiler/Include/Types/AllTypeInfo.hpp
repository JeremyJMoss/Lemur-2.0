#pragma once

/* === Imports === */

#include "Types/TypeInfo.hpp"
#include "Types/PrimitiveTypeInfo.hpp"
#include "Types/FunctionTypeInfo.hpp"
#include "Types/OwnershipTypeInfo.hpp"
#include "Types/CustomTypeInfo.hpp"
#include "Types/InferredTypeInfo.hpp"
#include "Types/NullTypeInfo.hpp"
#include "Types/UnresolvedTypeInfo.hpp"
#include <string>
#include <sstream>

/* === Utility === */

inline const std::string toString( const TypeInfo& typeInfo ) 
{
    switch ( typeInfo.kind )
    {
        case TypeKind::Primitive: 
        {
            const auto* prim = dynamic_cast<const PrimitiveTypeInfo*>( &typeInfo );
            return prim->name;
        }

        case TypeKind::Function: 
        {
            const auto* func = dynamic_cast<const FunctionTypeInfo*>( &typeInfo );
            std::ostringstream oss;
            oss << "fn(";
            for ( size_t i = 0; i < func->paramTypes.size(); ++i ) 
            {
                if (i > 0) oss << ", ";
                oss << toString( *func->paramTypes[i] );
            }
            oss << ") -> " << toString( *func->returnType );
            return oss.str();
        }

        case TypeKind::Ownership: 
        {
            const auto* owner = dynamic_cast<const OwnershipTypeInfo*>( &typeInfo );
            std::ostringstream oss;
            oss << toString( owner->ownership ) << " {";
            oss << toString( *owner->inner ) << "}";
            return oss.str();
        }

        case TypeKind::Custom: 
        {
            const auto* custom = dynamic_cast<const CustomTypeInfo*>( &typeInfo );
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
            return "<unknown type>";
        }
    }
}
