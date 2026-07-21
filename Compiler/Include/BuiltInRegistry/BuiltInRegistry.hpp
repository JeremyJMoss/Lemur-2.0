#pragma once

/* === Dependencies === */

#include <string>

/* === Imports === */

#include "Core/Ids.hpp"
#include "Types/BuiltInType.hpp"

/* === Forward Declarations === */

class CompilerContext;

/* === Built In Registry === */

class BuiltInRegistry
{
    public:
        static void initialize(
            CompilerContext& ctx
        );
    private:
        static void registerPrimitiveTypes( CompilerContext& context );
        static void registerPrimitive( CompilerContext& context, std::string_view name, PrimitiveType primitive );
};