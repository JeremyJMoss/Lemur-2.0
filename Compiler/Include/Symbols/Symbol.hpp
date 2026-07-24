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

enum class VariableStorage : uint8_t 
{
    Local,
    Parameter,
    Temporary,
};

struct VariableSymbol : Symbol {
    VariableStorage storage;
    bool isMutable;

    VariableSymbol(
        std::string name,
        TypeId typeId,
        VariableStorage storage,
        bool isMutable = false
    ) : Symbol( 
            std::move( name ), 
            SymbolKind::Variable, 
            typeId 
        ),
        storage( storage ),
        isMutable( isMutable ) {}
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