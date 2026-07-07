#include "Modules/ModuleResolver.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <fstream>
#include "Utils/Logger.hpp"
#include "SourceControl/SourceManager.hpp"
#include "Errors/ErrorReporter.hpp"
#include "Tokens/Tokenizer.hpp"

namespace fs = std::filesystem;
using FileId = std::size_t;

void ModuleResolver::buildModuleIndex( const fs::path& sourcePath )
{
    Logger::trace( "Started scanning file headers" );

    Logger::info( "Starting Module Resolution" );
    Logger::info( "Scanning file headers" );

    for ( const fs::directory_entry &file : fs::recursive_directory_iterator( sourcePath ) )
    {
        const fs::path path = file.path();
        const std::string pathStr = "'" + path.string() + "'";

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

        FileId fileId = m_srcManager.addFile( path );

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
            m_errReporter.report( RuntimeError(
                    "Error opening .lmur file",
                    ErrorSeverity::Fatal
                ) 
            );
            return;
        }

        auto maybeModuleIdentifier = Tokenizer::readModuleHeader( fileStream );

        if ( !maybeModuleIdentifier ) {
            m_errReporter.report( 
                ModuleHeaderError(
                    maybeModuleIdentifier.error().message + " for file path " + pathStr, 
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

        auto [it, inserted] = m_moduleIndex.emplace(maybeModuleIdentifier.value(), fileId);

        if ( !inserted )
        {
            m_errReporter.report(RuntimeError(
                "Duplicate module '" + maybeModuleIdentifier.value() + "'",
                ErrorSeverity::Fatal
            ));
        }
    }
}

std::optional<FileId> ModuleResolver::resolveModuleFileId( const std::string& moduleName ) const
{
    auto it = m_moduleIndex.find( moduleName );

    if (it == m_moduleIndex.end())
        return std::nullopt;

    return it->second;
}