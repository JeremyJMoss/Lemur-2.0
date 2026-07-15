#pragma once

/* === Dependencies ===*/

#include <string>
#include <ostream>

/* === Imports === */

#include "Core/Ids.hpp"

/* === Enum Declarations === */

enum class SymbolKind : u_int8_t
{
    Variable,
    Function,
    Type
};

/* === Symbols === */

struct Symbol 
{
    // Symbolid assigned via Symbol Table
    SymbolId id;
    const std::string name;
    SymbolKind kind;
    
    void setId( SymbolId symbolId ) { id = symbolId; }

    virtual ~Symbol() = default;

    Symbol( 
        std::string name,
        const SymbolKind& kind,
        TypeId typeId
    ) : id( InvalidSymbolId ),
        name( std::move( name ) ), 
        kind( kind ) {}
        
};

struct VariableSymbol : Symbol {
    bool isMutable;
    bool isTemporary;

    ScopeId scope;
    bool isParameter;

    VariableSymbol(
        std::string name,
        bool isMutable,
        TypeId typeId,
        bool isTemporary = false
    ) : Symbol( 
            std::move( name ), 
            SymbolKind::Variable, 
            typeId 
        ), 
        isMutable( isMutable ), 
        isTemporary( isTemporary ) {}
};

struct FunctionSymbol : Symbol {
    FunctionSymbol(
        std::string name,
        TypeId typeId
    ) : Symbol( 
            std::move(name), 
            SymbolKind::Function, 
            typeId 
        ) {}
};

struct TypeSymbol : Symbol {
    TypeSymbol(
        std::string name,
        TypeId typeId
    ) : Symbol( 
            std::move( name ), 
            SymbolKind::Type, 
            typeId 
        ) {}
};

/* === Utility === */

inline const std::string toString( SymbolKind kind ) 
{
    switch ( kind ) 
    {
        case SymbolKind::Variable:    return "Variable";
        case SymbolKind::Function:    return "Function";
        case SymbolKind::Type:        return "Type";
        default:                      return "Unknown";
    }
}

inline std::ostream& operator<< ( std::ostream& os, const Symbol& symbol ) 
{
    const std::string ind( 4, ' ' );
    os << "Symbol {" << std::endl;
    os << ind << "id: " << symbol.id << std::endl;
    os << ind << "name: " << symbol.name << std::endl;
    os << ind << "kind: " << toString( symbol.kind ) << std::endl;
    os << "}";
    return os;
}