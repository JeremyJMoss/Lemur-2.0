#pragma once

/* === Imports === */

#include "Core/Ids.hpp"

/* === Forward Declarations === */

class TypeTable;
class SymbolTable;

/* === Built In Registry === */

class BuiltinRegistry
{
public:

    static void initialize(
        TypeTable& types,
        SymbolTable& symbols,
        ScopeId builtinScope
    );
};