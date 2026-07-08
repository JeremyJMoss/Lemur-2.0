#include "Modules/ModuleResolver.hpp"

#include <filesystem>
#include <string>
#include <fstream>
#include "Utils/Logger.hpp"
#include "SourceControl/SourceManager.hpp"
#include "Errors/ErrorReporter.hpp"
#include "Tokens/Tokenizer.hpp"

namespace fs = std::filesystem;
using FileId = std::size_t;

void ModuleResolver::populate( const fs::path& sourcePath, ModuleTable& moduleTable )
{
    Logger::trace( "Started scanning file headers" );

    Logger::info( "Starting Module Resolution" );
    Logger::info( "Scanning file headers" );

    for ( const fs::directory_entry &file : fs::recursive_directory_iterator( sourcePath ) )
    {
        const fs::path path = file.path();
        const std::string pathStr = std::format( "'{}'", path.string() );

        if ( !file.is_regular_file() ) {
            continue;
        }

        if ( path.extension() != ".lmur" )
        {
            Logger::trace(
                "Skipping file",
                std::to_array<Attribute>({
                    { "Path", pathStr },
                    { "Reason", "Wrong extension" }
                })
            );
            continue;
        }

        auto maybeFileId = m_srcManager.addFile( path );

        if( !maybeFileId ) {
            m_errReporter.report(
                Diagnostic(
                    "Error saving file to Source Manager",
                    ErrorCategory::Linking,
                    ErrorSeverity::Fatal
                )
            );

            return;
        }

        FileId fileId = maybeFileId.value();

        Logger::debug(
            "File added to source manager",
            std::to_array<Attribute>({
                { "Path", pathStr },
                { "FileId", std::to_string( fileId ) }
            })
        );

        std::ifstream fileStream( path );

        if ( !fileStream.is_open() ) 
        {
            m_errReporter.report( 
                Diagnostic(
                    "Error opening .lmur file",
                    ErrorCategory::FileIO,
                    ErrorSeverity::Fatal
                ) 
            );
            return;
        }

        auto maybeModuleIdentifier = Tokenizer::readModuleHeader( fileStream );

        if ( !maybeModuleIdentifier ) {
            m_errReporter.report( 
                Diagnostic(
                    std::format(
                        "{} for file path {}", 
                        maybeModuleIdentifier.error().message, 
                        pathStr 
                    ), 
                    maybeModuleIdentifier.error().category,
                    maybeModuleIdentifier.error().severity
                ) 
            );
            continue;
        }

        Logger::debug(
            "Module discovered",
            std::to_array<Attribute>({
                { "Module", maybeModuleIdentifier.value() },
                { "Path", pathStr }
            })
        );

        bool inserted = moduleTable.add(fileId, maybeModuleIdentifier.value());

        if ( !inserted )
        {
            m_errReporter.report(
                Diagnostic(
                    std::format( 
                        "Duplicate module '{}'", 
                        maybeModuleIdentifier.value() 
                    ),
                    ErrorCategory::Linking,
                    ErrorSeverity::Fatal
            ));
        }
    }
}