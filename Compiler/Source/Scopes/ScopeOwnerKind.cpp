/* === Main Import === */

#include "Scopes/ScopeOwnerKind.hpp"

/* === Scope Owner Kind Methods === */

std::string_view toString( const ScopeOwnerKind& scopeOwnerKind ) {
    switch (scopeOwnerKind)
    {
        case ScopeOwnerKind::BuiltIn:   return "BuiltIn";
        case ScopeOwnerKind::Module:    return "Module";
        case ScopeOwnerKind::Function:  return "Function";
        case ScopeOwnerKind::Block:     return "Block";
        default:                        return "Unknown";
    }
}