/* === Main Import === */

#include "Symbols/SymbolKind.hpp"

/* === Utility Method === */

std::string_view toString( const SymbolKind& kind ) 
{
    switch ( kind ) 
    {
        case SymbolKind::Variable:    return "Variable";
        case SymbolKind::Function:    return "Function";
        case SymbolKind::Type:        return "Type";
        default:                      return "Unknown";
    }
}