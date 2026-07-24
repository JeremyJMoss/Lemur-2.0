#pragma once

/* === Dependencies === */

#include <cstdint>
#include <string>

/* === Scope Owner Kind === */

enum class ScopeOwnerKind 
{
    BuiltIn,
    Module,
    Function,
    Block,
    If,
    For
};

std::string_view toString( const ScopeOwnerKind& scopeOwnerKind );