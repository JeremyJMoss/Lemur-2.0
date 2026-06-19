#include "Driver/Driver.hpp"
#include "Tokens/Tokenizer.hpp"
#include "Errors/Errors.hpp"
#include "Utils/Logger.hpp"
#include "Debug/Debugger.hpp"
#include <array>

void Driver::compileProgram( std::string& filePath ) 
{
    try {
        // Get entry point
        Logger::debug( "Attempting to parse entry point file" );

        auto maybeParsedFile = parseFile( filePath );

        if ( !maybeParsedFile ) 
        {    
            m_errReporter.report(
                RuntimeError(
                    ErrorSeverity::Fatal,
                    maybeParsedFile.error()
                )
            );
        }

        if ( m_errReporter.hasErrors() )
        {
            m_errReporter.printAllDiagnostics();

            Logger::error( 
                std::to_string( m_errReporter.getErrCount() ) + " " + 
                maybeParsedFile.error() + ". Compilation terminated." 
            );

            return;
        }

        Logger::debug( "Parsed file" );
    }
    catch ( const FatalCompilerError& err )
    {
        m_errReporter.printAllDiagnostics();
        Logger::error( 
            std::to_string( m_errReporter.getErrCount() ) + 
            " error(s) detected. Compilation terminated." 
        );
    }
}

std::expected<void, std::string> Driver::parseFile( const std::string& filePath )
{
    auto tokens = m_tokenizer.tokenizeFile( filePath );

    Logger::debug( 
        std::to_string( tokens.size() ) + " tokens generated"
    );

    if ( m_errReporter.hasErrors() ) return std::unexpected( "Error(s) during lexing" );

    Logger::debug( 
        "Parsing tokens for file"
    );

    auto ast = m_parser.parse( std::move( tokens ) );
    if ( m_errReporter.hasErrors() ) return std::unexpected( "Error(s) during parsing" );

    Debugger debugger = Debugger();

    debugger.printASTTree(ast->statements);

    Logger::debug( 
        "AST generated with " +
        std::to_string( ast->statements.size() ) + 
        " top-level statements"
    );

    return {};
}