#pragma once

/* === Imports === */

#include <string>
#include <ostream>

using SymbolId = std::size_t;
using TypeId = std::size_t;

/* === Forward References === */

/* === Enums === */

// Symbol Types
enum class SymbolType 
{
    Variable,
    Function,
    Type
};

/* === Symbols === */

constexpr SymbolId InvalidSymbolId = static_cast<std::size_t>(-1);

struct Symbol 
{
    // Symbolid assigned via Symbol Table
    SymbolId m_id;
    TypeId m_typeId;
    std::string m_name;
    SymbolType m_kind;
    
    void setId( SymbolId symbolId ) { m_id = symbolId; }

    virtual ~Symbol() = default;

    Symbol( 
        std::string name,
        const SymbolType& kind,
        TypeId typeId
    ) : m_id( InvalidSymbolId ), 
        m_name( std::move( name ) ), 
        m_kind( kind ),
        m_typeId( typeId ) {}
        
};

struct VariableSymbol : Symbol {
    bool isMutable;
    bool isTemporary;

    VariableSymbol(
        std::string name,
        bool isMutable,
        TypeId typeId,
        bool isTemporary = false
    ) : Symbol( std::move(name), SymbolType::Variable, typeId ), isMutable(isMutable), isTemporary(isTemporary) {}
};

struct FunctionSymbol : Symbol {
    FunctionSymbol(
        std::string name,
        TypeId typeId
    ) : Symbol( std::move(name), SymbolType::Function, typeId ) {}
};

struct TypeSymbol : Symbol {
    TypeSymbol(
        const std::string& name,
        TypeId typeId
    ) : Symbol( name, SymbolType::Type, typeId ) {}
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
    os << ind << "id: " << symbol.m_id << std::endl;
    os << ind << "name: " << symbol.m_name << std::endl;
    os << ind << "kind: " << toString( symbol.m_kind ) << std::endl;
    os << "}";
    return os;
}