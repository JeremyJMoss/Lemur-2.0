#include "Driver/Driver.hpp"
#include "Tokens/Tokenizer.hpp"
#include "Parser/Parser.hpp"
#include "Errors/Errors.hpp"
#include "Utils/Logger.hpp"
#include "AST/ASTPrinter.hpp"
#include "Modules/ModuleResolver.hpp"
#include "Config/Config.hpp"
#include "Utils/Output.hpp"
#include <array>
#include <chrono>

namespace chrono = std::chrono;

void Driver::compileProgram() 
{
    try {
        compile();
        m_errReporter.printAllDiagnostics();
    } catch (const FatalCompilerError& error) {
        m_errReporter.printAllDiagnostics();
    }
}

void Driver::compile() {
    // Get entry point
    Logger::debug( "Attempting to parse entry point file" );

    ModuleResolver moduleResolver(m_srcManager, m_errReporter);

    moduleResolver.buildModuleIndex(m_config.sourcePath);

    auto fileId = moduleResolver.resolveModuleFileId(m_config.entryModule);

    if ( !fileId ) {
        m_errReporter.report(
            ModuleHeaderError("Unable to find entry module \"" + m_config.entryModule + "\" within declared module", ErrorSeverity::Fatal)
        );
        
        Logger::error( 
            "Unable to find entry module \"" + m_config.entryModule + "\" within declared modules" 
        );

        return;
    }

    auto compUnit = std::make_unique<CompilationUnit>( fileId.value() );

    auto tokenStart = chrono::high_resolution_clock::now();

    tokenizeCompilationUnit( *compUnit );

    auto tokenEnd = chrono::high_resolution_clock::now();
    auto tokenDuration = duration_cast<chrono::microseconds>( tokenEnd - tokenStart );

    std::cout << "Lexer Execution time: " << tokenDuration.count() << " µs\n";

    if ( m_errReporter.hasErrors() )
    {
        m_errReporter.printAllDiagnostics();

        Logger::error( 
            std::to_string( m_errReporter.getErrCount() ) + " lexing error(s) found. Compilation terminated." 
        );

        return;
    }

    auto parserStart = chrono::high_resolution_clock::now();

    parseCompilationUnit( *compUnit );

    auto parserEnd = chrono::high_resolution_clock::now();
    auto parserDuration = duration_cast<chrono::microseconds>( parserEnd - parserStart );

    std::cout << "Parser Execution time: " << parserDuration.count() << " µs\n";

    if ( m_errReporter.hasErrors() )
    {
        m_errReporter.printAllDiagnostics();

        Logger::error( 
            std::to_string( m_errReporter.getErrCount() ) + " parser error(s) found. Compilation terminated." 
        );

        return;
    }

    if (m_config.emitAST) {
        ASTPrinter astPrinter = ASTPrinter();
        astPrinter.print( compUnit->readStatements() );
    }

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
    fs::path filePath = m_srcManager.getFilePath(compUnit.getFileId());
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