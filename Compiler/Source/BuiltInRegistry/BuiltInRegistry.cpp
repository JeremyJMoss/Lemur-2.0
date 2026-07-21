/* === Main Import === */

#include "BuiltInRegistry/BuiltInRegistry.hpp"

/* === Dependencies === */

#include <format>

/* === Imports === */

#include "Core/CompilerContext.hpp"
#include "Types/PrimitiveInfo.hpp"
#include "Types/Type.hpp"

/* === Built In Registry === */

void BuiltInRegistry::initialize(
    CompilerContext& ctx
)
{
    registerPrimitiveTypes( ctx );
}

void BuiltInRegistry::registerPrimitiveTypes(
    CompilerContext& ctx
) 
{
    registerPrimitive(ctx, "int", PrimitiveType::Int);
    registerPrimitive(ctx, "int32", PrimitiveType::Int32);
    registerPrimitive(ctx, "int16", PrimitiveType::Int16);
    registerPrimitive(ctx, "int8", PrimitiveType::Int8);
    registerPrimitive(ctx, "float", PrimitiveType::Float);
    registerPrimitive(ctx, "float32", PrimitiveType::Float32);
    registerPrimitive(ctx, "bool", PrimitiveType::Bool);
    registerPrimitive(ctx, "char", PrimitiveType::Char);
    registerPrimitive(ctx, "uint", PrimitiveType::UInt);
    registerPrimitive(ctx, "uint32", PrimitiveType::UInt32);
    registerPrimitive(ctx, "uint16", PrimitiveType::UInt16);
    registerPrimitive(ctx, "uint8", PrimitiveType::UInt8);
}

void BuiltInRegistry::registerPrimitive(
    CompilerContext& ctx,
    std::string_view name,
    PrimitiveType type
)
{
   TypeId typeId = ctx.addType(
        Type(TypeKind::Primitive, TypeState::Resolved, TypeOrigin::Builtin, PrimitiveInfo{type})
   );

   SymbolId symbolId = ctx.addSymbol(
        TypeSymbol( std::string( name ), typeId )
   );

   auto result = ctx.declareInScope( ctx.getBuiltInScope(), name, symbolId );

   if ( !result )
   {
        throw InternalCompilerError( 
            std::format(
                "Duplicate builtin declaration '{}'.\nBuiltinRegistry attempted to register the same primitive twice.\nPlease report this bug.",
                name
            )
        );
   }
}