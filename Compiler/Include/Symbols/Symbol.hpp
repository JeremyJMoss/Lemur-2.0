#pragma once

/* === Dependencies ===*/

#include <string>

/* === Imports === */

#include "Core/Ids.hpp"
#include "Symbols/SymbolKind.hpp"
#include "SourceControl/SourceLocation.hpp"

/* === Symbols === */

struct Symbol 
{
    // Symbolid assigned via Symbol Table
    SymbolId id{};
    
    std::string name;
    const SymbolKind kind;
    TypeId typeId;

    ScopeId scope{};

    SourceRange declaration;
    
    void setId( SymbolId symbolId ) { id = symbolId; }

    virtual ~Symbol() = default;

    protected:
        Symbol( 
            std::string name,
            SymbolKind kind,
            TypeId typeId
        ) : name( std::move( name ) ), 
            kind( kind ),
            typeId( typeId ) {}
};

struct VariableSymbol : Symbol {
    bool isMutable;
    bool isTemporary;
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