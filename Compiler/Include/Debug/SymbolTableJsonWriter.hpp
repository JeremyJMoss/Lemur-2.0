#pragma once

/* === Dependencies === */

#include <filesystem>

namespace fs = std::filesystem;

/* === Imports === */

#include "Debug/JsonWriter.hpp"
#include "Debug/TypeTableJsonWriter.hpp"

/* === Forward Declarations === */

class CompilerContext;
struct Symbol;
struct VariableSymbol;

/* === Symbol Table Json Writer === */

class SymbolTableJsonWriter
{
    public:
        SymbolTableJsonWriter( JsonWriter& jsonWriter )
            : m_writer( jsonWriter ), m_type_writer( m_writer ) {}

        void write( CompilerContext& ctx, const fs::path& outputPath );

    private:
        JsonWriter& m_writer;
        TypeTableJsonWriter m_type_writer;

        void writeSymbol( const Symbol& sym, CompilerContext& ctx );

        void writeKindSpecificFields( const Symbol& sym, CompilerContext& ctx );

        void writeVariableSymbolFields( const VariableSymbol& varSymbol, CompilerContext& ctx );

        void writeFunctionSymbolFields( const FunctionSymbol& funSymbol, CompilerContext& ctx );

        void writeTypeSymbolFields( const TypeSymbol& typeSymbol, CompilerContext& ctx );
};