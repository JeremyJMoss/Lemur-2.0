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

    ScopeId scope{};

    SourceRange declaration;
    
    void setId( SymbolId symbolId ) { id = symbolId; }

    virtual ~Symbol() = default;

    protected:
        Symbol( 
            std::string name,
            SymbolKind kind
        ) : name( std::move( name ) ), 
            kind( kind ) {}
};

struct TypedSymbol : Symbol
{
    TypeId typeId;

    protected:
        TypedSymbol(
            std::string name,
            SymbolKind kind,
            TypeId typeId
        ) : Symbol(
                std::move( name ),
                kind
            ),
            typeId( typeId ) {}
};

enum class VariableStorage : uint8_t 
{
    Local,
    Parameter,
    Temporary,
};

inline std::string toString( const VariableStorage storage )
{
    switch( storage )
    {
        case VariableStorage::Local:     return "Local";
        case VariableStorage::Parameter: return "Parameter";
        case VariableStorage::Temporary: return "Temporary";
        default:                         return "Unknown";
    }
}

struct VariableSymbol : TypedSymbol 
{
    VariableStorage storage;
    bool isMutable;

    VariableSymbol(
        std::string name,
        TypeId typeId,
        VariableStorage storage,
        bool isMutable = false
    ) : TypedSymbol( 
            std::move( name ), 
            SymbolKind::Variable,
            typeId
        ),
        storage( storage ),
        isMutable( isMutable ) {}
};

struct FunctionSymbol : TypedSymbol 
{
    FunctionSymbol(
        std::string name,
        TypeId typeId
    ) : TypedSymbol( 
            std::move( name ), 
            SymbolKind::Function,
            typeId
        ) {}
};

struct TypeSymbol : TypedSymbol 
{
    TypeSymbol(
        std::string name,
        TypeId typeId
    ) : TypedSymbol( 
            std::move( name ), 
            SymbolKind::Type,
            typeId
        ) {}
};

struct ImportSymbol : Symbol
{
    SymbolId target;

    ImportSymbol(
        std::string name,
        SymbolId target
    )
    : Symbol (
            std::move( name ),
            SymbolKind::Import
        ),
        target( target ) {}
};

struct ModuleSymbol : Symbol
{
    ModuleId target;

    ModuleSymbol(
        std::string name,
        ModuleId target
    ) : Symbol (
        std::move( name ),
        SymbolKind::Module
    ),
    target( target ) {}
};