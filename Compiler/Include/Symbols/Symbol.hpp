#pragma once

/* === Imports === */

#include <string>
#include <ostream>
#include <vector>
#include <memory>
#include <optional>
#include "Types/Type.hpp"
#include "Types/FunctionType.hpp"
#include "AST/ASTNode.hpp"

using SymbolId = std::size_t;

/* === Forward References === */

/* === Enums === */

// Symbol Types
enum class SymbolType 
{
    Variable,
    Function,
    Type
};

/* === Symbol Structs === */

struct Symbol 
{
    static inline SymbolId nextId = 0;
    SymbolId id;
    std::string name;
    SymbolType kind;
    virtual ~Symbol() = default;

    Symbol( 
        const std::string& name,
        const SymbolType& kind
    ) 
        : id( nextId++ ),
          name( std::move( name ) ), 
          kind( kind ) {}
};

struct VariableSymbol : Symbol {
    std::shared_ptr<Type> type;
    bool isMutable;
    bool isTemporary;

    VariableSymbol(
        const std::string& name,
        bool isMutable,
        std::shared_ptr<Type> type = nullptr,
        bool isTemporary = false
    ) : Symbol( name, SymbolType::Variable ), type(type), isMutable(isMutable), isTemporary(isTemporary) {}
};

struct FunctionSymbol : Symbol {
    std::shared_ptr<FunctionType> type;

    FunctionSymbol(
        const std::string& name,
        std::shared_ptr<FunctionType> type = nullptr
    ) : Symbol( name, SymbolType::Function ), type(type) {}
};

struct TypeSymbol : Symbol {
    std::shared_ptr<Type> type;

    TypeSymbol(
        const std::string& name,
        std::shared_ptr<Type> type = nullptr
    ) : Symbol( name, SymbolType::Type ), type( type ) {}
};

/* === Utility === */

inline const std::string toString( const SymbolType& kind ) 
{
    switch ( kind ) 
    {
        case SymbolType::Variable:    return "Variable";
        case SymbolType::Function:    return "Function";
        case SymbolType::Type:        return "Type";
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