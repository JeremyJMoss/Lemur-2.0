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
        case ScopeOwnerKind::If:        return "If";
        case ScopeOwnerKind::For:       return "For";
        default:                        return "Unknown";
    }
}