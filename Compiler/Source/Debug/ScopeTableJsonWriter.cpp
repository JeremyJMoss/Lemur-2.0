/* === Main Import === */

#include "Debug/ScopeTableJsonWriter.hpp"

/* === Imports === */

#include "Errors/Errors.hpp"

/* === Scope Table Json Writer Methods === */

void ScopeTableJsonWriter::write( const CompilerContext& ctx, const fs::path& outputPath, const std::string_view moduleName )
{
    // Create the directory (and any missing parent directories)
    std::error_code ec;
    fs::create_directories( outputPath, ec );

    if ( ec ) throw InternalCompilerError( "Failed to create output directory: " + ec.message() );

    std::string fileName = std::string( moduleName ) + "_scope_table.json";
    std::ofstream outFile( outputPath / fileName, std::ios::out | std::ios::binary );

    if ( !outFile ) throw InternalCompilerError( "Failed to open output file." );

    // Create 1MB buffer
    outFile.rdbuf()->pubsetbuf( nullptr, 1 << 20 );

    m_out = &outFile;

    *m_out << "[\n";

    

    *m_out << "\n]\n";

    outFile.close();
}