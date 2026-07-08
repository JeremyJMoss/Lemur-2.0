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
    try 
    {
        compile();

        if ( m_errReporter.hasDiagnostics() ) m_errReporter.printAllDiagnostics();
    } 
    catch ( const InternalCompilerError& error )
    {
        Output::error( error.what() );
    } 
    catch ( const CompilationAborted& error )
    {
        m_errReporter.printAllDiagnostics();
        Output::error( error.what() );
    } 
    catch ( const std::runtime_error& error )
    {
        Output::error(
            std::format(
                "Unknown runtime error occurred: {}", 
                error.what()
            )
        );
    } 
    catch ( const std::exception& error )
    {
        Output::error("An unknown error occured");
    }
}

void Driver::compile() {
    // Get entry point
    Logger::debug( "Attempting to parse entry point file" );

    ModuleResolver moduleResolver( m_srcManager, m_errReporter );

    moduleResolver.buildModuleIndex( m_config.sourcePath );

    auto maybeFileId = moduleResolver.resolveModuleFileId( m_config.entryModule );

    if ( !maybeFileId ) {
        m_errReporter.report(
            Diagnostic(
                std::format(
                    "Unable to find entry module '{}' within declared module",
                    m_config.entryModule
                ),
                ErrorCategory::Linking,
                ErrorSeverity::Fatal
            )
        );
        
        Logger::error(
            std::format( 
                "Unable to find entry module '{}' within declared modules",
                m_config.entryModule
            )
        );

        return;
    }

    FileId fileId = maybeFileId.value();

    m_srcManager.setModuleName(fileId, m_config.entryModule );

    auto compUnit = std::make_unique<CompilationUnit>( fileId, m_config.entryModule );

    auto tokenStart = chrono::high_resolution_clock::now();

    tokenizeCompilationUnit( *compUnit );

    auto tokenEnd = chrono::high_resolution_clock::now();
    auto tokenDuration = duration_cast<chrono::microseconds>( tokenEnd - tokenStart );

    Output::success( std::format( "Lexer Execution time: {} µs", tokenDuration.count() ) );

    if ( m_errReporter.hasErrors() )
    {
        Logger::error( 
            std::format( 
                "{} lexing error(s) found. Compilation terminated.", 
                m_errReporter.getErrCount() 
            )
        );

        return;
    }

    auto parserStart = chrono::high_resolution_clock::now();

    parseCompilationUnit( *compUnit );

    auto parserEnd = chrono::high_resolution_clock::now();
    auto parserDuration = duration_cast<chrono::microseconds>( parserEnd - parserStart );

    Output::success( 
        std::format( 
            "Parser Execution time: {} µs", 
            parserDuration.count() 
        ) 
    );

    if ( m_errReporter.hasErrors() )
    {
        Logger::error( 
            std::format( 
                "{} parser error(s) found. Compilation terminated.",  
                m_errReporter.getErrCount() 
            )
        );

        return;
    }

    if ( m_config.emitAST ) {
        ASTPrinter astPrinter = ASTPrinter();
        astPrinter.print( compUnit->readStatements(), m_config.outputPath );
    }

    Logger::debug( 
        std::format(
            "AST generated with {} top-level statements",
            compUnit->readStatements().size()
        )
    );

    Logger::debug( "Parsed file" );

    // free all memory within Compilation Unit
    compUnit->freeArena();
}

void Driver::tokenizeCompilationUnit( CompilationUnit& compUnit ) {
    fs::path filePath = m_srcManager.getFilePath( compUnit.getFileId() );
    std::ifstream fileStream( filePath );

    if ( !fileStream.is_open() ) 
    {
        m_errReporter.report( 
            Diagnostic (
                "Error opening .lmur file",
                ErrorCategory::FileIO,
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
        std::format( 
            "{} tokens generated", 
            compUnit.getTokenCount() 
        )
    );
} 

void Driver::parseCompilationUnit( CompilationUnit& compUnit ) {
    
    Logger::debug( 
        "Parsing tokens for file"
    );

    Parser parser = Parser( compUnit, m_errReporter );

    parser.parse();
}