#pragma once

/* === Dependencies === */

#include <string>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

/* === Imports === */

#include "Debug/JsonWriter.hpp"
#include "Types/Type.hpp"

/* === Forward Declarations === */

class CompilerContext;
struct FunctionInfo;

/* === Type Table Json Writer === */

class TypeTableJsonWriter
{
    public:
        TypeTableJsonWriter( JsonWriter& jsonWriter )
            : m_writer( jsonWriter ) {}

        void write( CompilerContext& ctx, const fs::path& outputPath );

        void writeTypeField( std::string_view label, const Type& type, CompilerContext& ctx, bool hasComma = true );

    private:
        JsonWriter& m_writer;

        void writeType( const Type& type, CompilerContext& ctx );

        void writeTypeData( const TypeData& data, CompilerContext& ctx );

        void writePrimitiveInfo( const PrimitiveInfo& primInfo );

        void writeArrayInfo( const ArrayInfo& arrayInfo, CompilerContext& ctx );

        void writeOwnershipInfo( const OwnershipInfo& ownerInfo, CompilerContext& ctx );

        void writeFunctionInfo( const FunctionInfo& funInfo, CompilerContext& ctx );

};