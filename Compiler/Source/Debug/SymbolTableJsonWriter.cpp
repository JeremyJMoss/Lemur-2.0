/* === Main Import === */

#include "Debug/SymbolTableJsonWriter.hpp"

/* === Imports === */

#include "Errors/Errors.hpp"
#include "Core/CompilerContext.hpp"
#include "Symbols/Symbol.hpp"
#include "Core/Ids.hpp"
#include "Debug/TypeTableJsonWriter.hpp"

/* === Symbol Table Json Writer Methods === */

void SymbolTableJsonWriter::writeSymbol( const Symbol& sym, CompilerContext& ctx )
{
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", sym.id.value );
    m_writer.writeField( "name", sym.name );
    writeKindSpecificFields( sym, ctx );
    m_writer.writeField( "kind", toString( sym.kind ) );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void SymbolTableJsonWriter::writeKindSpecificFields( const Symbol& sym, CompilerContext& ctx )
{
    switch ( sym.kind )
    {
        case SymbolKind::Variable: {
            writeVariableSymbolFields( static_cast<const VariableSymbol&>( sym ), ctx );
            return;
        }
        case SymbolKind::Function: {
            writeFunctionSymbolFields( static_cast<const FunctionSymbol&>( sym ), ctx );
            return;
        }
        case SymbolKind::Type: {
            writeTypeSymbolFields( static_cast<const TypeSymbol&>( sym ), ctx );
            return;
        }

    }
}

void SymbolTableJsonWriter::writeVariableSymbolFields( const VariableSymbol& varSymbol, CompilerContext& ctx )
{
    if ( varSymbol.typeId.valid() ){
        m_type_writer.writeTypeField( "type", ctx.types().get(varSymbol.typeId), ctx );
    }
    else
    {
        m_writer.writeField( "type", "\"Unresolved\"" );
    }
    m_writer.writeField( "stored", toString( varSymbol.storage ) );
    m_writer.writeField( "isMutable", ( varSymbol.isMutable ? "true" : "false" ) );
}

void SymbolTableJsonWriter::writeFunctionSymbolFields( const FunctionSymbol& funSymbol, CompilerContext& ctx )
{
    if ( funSymbol.typeId.valid() ){
        m_type_writer.writeTypeField( "type", ctx.types().get(funSymbol.typeId), ctx );
    }
    else
    {
        m_writer.writeField( "type", "\"Unresolved\"" );
    }
}

void SymbolTableJsonWriter::writeTypeSymbolFields( const TypeSymbol& typeSymbol, CompilerContext& ctx )
{
    if ( typeSymbol.typeId.valid() ){
        m_type_writer.writeTypeField( "type", ctx.types().get(typeSymbol.typeId), ctx );
    }
    else
    {
        m_writer.writeField( "type", "\"Unresolved\"" );
    }
}



void SymbolTableJsonWriter::write( CompilerContext& ctx, const fs::path& outputPath )
{
    // Create the directory (and any missing parent directories)
    std::error_code ec;
    fs::create_directories( outputPath, ec );

    if ( ec ) throw InternalCompilerError( "Failed to create output directory: " + ec.message() );

    std::string fileName = "symbol_table.json";
    std::ofstream outFile( outputPath / fileName, std::ios::out | std::ios::binary );

    if ( !outFile ) throw InternalCompilerError( "Failed to open output file." );

    // Create 1MB buffer
    outFile.rdbuf()->pubsetbuf( nullptr, 1 << 20 );

    m_writer.setOutFile( outFile );

    auto symbols = ctx.symbols().getAll();

    m_writer.writeRaw("[");
    m_writer.writeNewLine();

    for ( std::size_t i = 0; i < symbols.size(); ++i )
    {
        if ( i != 0 )
        {
            m_writer.writeRaw(",");
            m_writer.writeNewLine();
        }

        auto symbol = symbols[i];

        m_writer.increaseIndent();
        writeSymbol( *symbol, ctx );
        m_writer.decreaseIndent();
    }

    m_writer.writeNewLine();
    m_writer.writeRaw("]");
    m_writer.writeNewLine();

    outFile.close();
}
