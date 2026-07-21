/* === Main Import === */

#include "Driver/Driver.hpp"

/* === Dependencies === */

#include <array>
#include <chrono>
#include <format>

namespace chrono = std::chrono;

/* === Imports === */

#include "Tokens/Tokenizer.hpp"
#include "Parser/Parser.hpp"
#include "Errors/Errors.hpp"
#include "Logging/Logger.hpp"
#include "AST/ASTPrinter.hpp"
#include "Modules/ModuleHeaderScanner.hpp"
#include "Config/CompilerConfig.hpp"
#include "Utils/Output.hpp"

/* === Driver Methods === */

void Driver::compileProgram()
{
    try 
    {
        compile();

        if ( m_ctx.errors().hasDiagnostics() ) m_ctx.errors().printAllDiagnostics();
    } 
    catch ( const InternalCompilerError& error )
    {
        Output::error( error.what() );
    } 
    catch ( const CompilationAborted& error )
    {
        m_ctx.errors().printAllDiagnostics();
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

void Driver::compile() 
{
    // Get entry point
    Logger::debug( "Attempting to parse entry point file" );

    m_ctx.initialize();

    auto moduleScanStart = chrono::high_resolution_clock::now();

    ModuleHeaderScanner scanner = ModuleHeaderScanner( m_ctx );

    auto maybeScanSuccessful = scanner.scan( m_config.sourcePath, m_ctx );

    if ( !maybeScanSuccessful ) 
    {
        m_ctx.errors().report(
            Diagnostic(
                "Failed to scan modules within source path folder tree",
                ErrorCategory::Linking,
                ErrorSeverity::Fatal
            )
        );
    }

    auto maybeModule = m_ctx.modules().find( m_config.entryModule );

    if ( maybeModule == nullptr ) 
    {
        m_ctx.errors().report(
            Diagnostic(
                std::format(
                    "Unable to find entry module '{}' within declared modules",
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

    ModuleId entryModuleId = maybeModule->id;

    auto reachableModules = m_ctx.modules().resolveImports( entryModuleId );

    auto moduleScanEnd = chrono::high_resolution_clock::now();
    auto ModuleScanDuration = duration_cast<chrono::microseconds>( moduleScanEnd - moduleScanStart );

    Output::success( 
        std::format( 
            "Module Scan Execution time: {} µs", 
            ModuleScanDuration.count() 
        ) 
    );

    if ( !reachableModules ) 
    {
        m_ctx.errors().report(
            reachableModules.error()
        );

        return;
    }

    // TODO do something about reachable vs unreachable modules maybe throw a warning on the screen for
    // unreachable modules from the enrty point

    std::vector<ModuleId> parseOrder = m_ctx.modules().buildParseOrder( entryModuleId );
    
    for ( ModuleId moduleId : parseOrder )
    {
        auto compUnit = std::make_unique<CompilationUnit>( m_ctx.modules().get( moduleId ), m_ctx );

        auto tokenStart = chrono::high_resolution_clock::now();

        tokenizeCompilationUnit( *compUnit );

        auto tokenEnd = chrono::high_resolution_clock::now();
        auto tokenDuration = duration_cast<chrono::microseconds>( tokenEnd - tokenStart );

        Output::success( std::format( "Lexer Execution time: {} µs", tokenDuration.count() ) );

        if ( m_ctx.errors().hasErrors() )
        {
            Logger::error( 
                std::format( 
                    "{} lexing error(s) found. Compilation terminated.", 
                    m_ctx.errors().getErrCount() 
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

        if ( m_ctx.errors().hasErrors() )
        {
            Logger::error( 
                std::format( 
                    "{} parser error(s) found. Compilation terminated.",  
                    m_ctx.errors().getErrCount() 
                )
            );

            return;
        }

        if ( m_config.emitAST ) 
        {
            ASTPrinter astPrinter = ASTPrinter();
            astPrinter.print( compUnit->ast().getStatements(), m_config.outputPath, compUnit->getModuleName() );
        }

        Logger::debug( 
            std::format(
                "AST generated for module {} with {} top-level statements",
                compUnit->getModuleName(),
                compUnit->ast().getStatements().size()
            )
        );

        Logger::debug( "Parsed module" );

        Logger::debug( "Declaration Pass" );



        // free all memory within Compilation Unit
        compUnit->freeArena();
    }

    m_ctx.freeArena();
}

void Driver::tokenizeCompilationUnit( CompilationUnit& compUnit ) 
{
    fs::path filePath = m_ctx.source().getFilePath( compUnit.getFileId() );
    std::ifstream fileStream( filePath );

    if ( !fileStream.is_open() ) 
    {
        m_ctx.errors().report( 
            Diagnostic (
                "Error opening .lmur file",
                ErrorCategory::FileIO,
                ErrorSeverity::Fatal
            ) 
        );
        return;
    }

    Logger::debug( 
        "Lexing tokens for module " + std::string( compUnit.getModuleName() )
    );

    Tokenizer tokenizer = Tokenizer( compUnit );

    tokenizer.tokenizeStream( fileStream );

    if ( m_ctx.errors().hasErrors() ) return;

    Logger::trace( 
        std::format( 
            "{} tokens generated", 
            compUnit.getTokenCount() 
        )
    );
} 

void Driver::parseCompilationUnit( CompilationUnit& compUnit ) 
{
    
    Logger::debug( 
        "Parsing tokens for module " + std::string( compUnit.getModuleName() )
    );

    Parser parser = Parser( compUnit );

    parser.parse();
}