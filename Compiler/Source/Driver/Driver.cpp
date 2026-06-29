#include "Driver/Driver.hpp"
#include "Tokens/Tokenizer.hpp"
#include "Errors/Errors.hpp"
#include "Utils/Logger.hpp"
#include "Debug/Debugger.hpp"
#include <array>

void Driver::compileProgram( std::string& filePath ) 
{
    // Get entry point
    Logger::debug( "Attempting to parse entry point file" );

    FileId fileId = m_srcManager.addFile(filePath);
    auto compUnit = std::make_unique<CompilationUnit>( fileId );

    tokenizeCompilationUnit( *compUnit );

    if ( m_errReporter.hasErrors() )
    {
        m_errReporter.printAllDiagnostics();

        Logger::error( 
            std::to_string( m_errReporter.getErrCount() ) + " lexing error(s) found. Compilation terminated." 
        );

        return;
    }

    parseCompilationUnit( *compUnit );

    if ( m_errReporter.hasErrors() )
    {
        m_errReporter.printAllDiagnostics();

        Logger::error( 
            std::to_string( m_errReporter.getErrCount() ) + " parser error(s) found. Compilation terminated." 
        );

        return;
    }

    Debugger debugger = Debugger();

    debugger.printASTTree( compUnit->readStatements() );

    Logger::debug( 
        "AST generated with " +
        std::to_string( compUnit->readStatements().size() ) + 
        " top-level statements"
    );

    Logger::debug( "Parsed file" );

    // free all memory within Compilation Unit
    compUnit->freeArena();
}

void Driver::tokenizeCompilationUnit( CompilationUnit& compUnit ) {
    const std::string& filePath = m_srcManager.getFilePath(compUnit.getFileId());
    std::ifstream fileStream( filePath );

    if ( !fileStream.is_open() ) 
    {
        m_errReporter.report( RuntimeError(
                "Error opening .lmur file",
                ErrorSeverity::Fatal
            ) 
        );
        return;
    }

    Logger::debug( 
        "Lexing tokens for file"
    );

    Tokenizer tokenizer = Tokenizer( compUnit, m_errReporter );

    tokenizer.tokenizeStream( fileStream );

    if ( m_errReporter.hasErrors() ) return;

    Logger::trace( 
        std::to_string( compUnit.getTokenCount() ) + " tokens generated"
    );
} 

void Driver::parseCompilationUnit( CompilationUnit& compUnit ) {
    
    Logger::debug( 
        "Parsing tokens for file"
    );

    Parser parser = Parser( compUnit, m_errReporter );

    parser.parse();

    if ( m_errReporter.hasErrors() ) return;
}