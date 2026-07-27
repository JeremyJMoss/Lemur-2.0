/* === Main Import === */

#include "Debug/TypeTableJsonWriter.hpp"

/* === Dependencies === */

#include <span>

/* === Imports === */

#include "Errors/Errors.hpp"
#include "Core/CompilerContext.hpp"
#include "Types/FunctionInfo.hpp"

/* === Scope Table Json Writer Methods === */

void TypeTableJsonWriter::writeTypeData( const TypeData& data, CompilerContext& ctx )
{
    std::visit( [&] ( const auto& val )
    {
        using T = std::decay_t<decltype( val )>;

        if constexpr ( std::is_same_v<T, PrimitiveInfo> )
        {
            writePrimitiveInfo( val );
        }
        else if constexpr (std::is_same_v<T, ArrayInfo>)
        {
            writeArrayInfo( val, ctx );
        }
        else if constexpr (std::is_same_v<T, OwnershipInfo>)
        {
            writeOwnershipInfo( val, ctx );
        }
        else if constexpr (std::is_same_v<T, FunctionInfo>)
        {
            writeFunctionInfo( val, ctx );
        }
    }, data );
}

void TypeTableJsonWriter::writePrimitiveInfo( const PrimitiveInfo& primInfo )
{
    m_writer.writeField( "name", toString( primInfo.name ) );
}

void TypeTableJsonWriter::writeArrayInfo( const ArrayInfo& arrayInfo, CompilerContext& ctx )
{
    writeTypeField( "type", ctx.types().get( arrayInfo.elementType ), ctx, arrayInfo.capacity.has_value() );
    if ( arrayInfo.capacity.has_value() )
    {
        m_writer.writeField( "capacity", arrayInfo.capacity.value() );
    }
}

void TypeTableJsonWriter::writeOwnershipInfo( const OwnershipInfo& ownerInfo, CompilerContext& ctx )
{
    writeTypeField( "inner", ctx.types().get( ownerInfo.inner ), ctx );
    m_writer.writeField( "ownership", toString( ownerInfo.ownership ) );
}

void TypeTableJsonWriter::writeFunctionInfo( const FunctionInfo& funInfo, CompilerContext& ctx )
{
    writeTypeField( "returnType", ctx.types().get( funInfo.returnType ), ctx );
    m_writer.writeArrayField( 
        "parameters", 
        funInfo.paramTypes,
        [&] ( const TypeId id )
        {
            writeType( ctx.types().get(id), ctx );
        },
        false
    );
}

void TypeTableJsonWriter::writeType( const Type& type, CompilerContext& ctx )
{
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", type.id.value );
    writeTypeData( type.data, ctx );
    m_writer.writeField( "kind", toString( type.kind ), false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void TypeTableJsonWriter::writeTypeField( std::string_view label, const Type& type, CompilerContext& ctx, bool hasComma )
{
    m_writer.writeIndent();
    m_writer.writeRaw( std::format("\"{}\": \n", label) );
    m_writer.increaseIndent();
    writeType( type, ctx );
    if ( hasComma ) {
        m_writer.writeRaw(",");
    }
    m_writer.writeNewLine();
    m_writer.decreaseIndent();
}


void TypeTableJsonWriter::write( CompilerContext& ctx, const fs::path& outputPath )
{
    // Create the directory (and any missing parent directories)
    std::error_code ec;
    fs::create_directories( outputPath, ec );

    if ( ec ) throw InternalCompilerError( "Failed to create output directory: " + ec.message() );

    std::string fileName = "type_table.json";
    std::ofstream outFile( outputPath / fileName, std::ios::out | std::ios::binary );

    if ( !outFile ) throw InternalCompilerError( "Failed to open output file." );

    // Create 1MB buffer
    outFile.rdbuf()->pubsetbuf( nullptr, 1 << 20 );

    m_writer.setOutFile( outFile );

    auto types = ctx.types().getAll();

    m_writer.writeRaw("[");
    m_writer.writeNewLine();

    for ( std::size_t i = 0; i < types.size(); ++i )
    {
        if ( i != 0 )
        {
            m_writer.writeRaw(",");
            m_writer.writeNewLine();
        }

        auto type = types[i];

        m_writer.increaseIndent();
        writeType( *type, ctx );
        m_writer.decreaseIndent();
    }

    m_writer.writeNewLine();
    m_writer.writeRaw("]");
    m_writer.writeNewLine();

    outFile.close();
}