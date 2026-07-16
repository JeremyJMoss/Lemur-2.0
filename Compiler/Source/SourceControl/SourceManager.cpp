#include "SourceControl/SourceManager.hpp"
#include "Logging/Logger.hpp"
#include "Modules/ModuleHeader.hpp"
#include <array>
#include <string>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace fs = std::filesystem;

std::expected<FileId, Diagnostic> SourceManager::addFile( const fs::path& filePath )
{
    const std::string pathStr = std::format( "'{}'", filePath.string() );

    const std::array pathAttr { 
        Attribute{ "Path", pathStr } 
    };

    auto it = m_pathToId.find( filePath );

    if ( it != m_pathToId.end() ) 
    {
        Logger::trace( 
            "Found existing file in source manager",
            pathAttr
        );
        return it->second;
    }

    std::ifstream file( filePath, std::ios::binary );

    if ( !file.is_open() ) 
    {
        Logger::trace( 
            "Could not open file when attempting to add file to source manager",
            pathAttr
        );

        return std::unexpected(
            Diagnostic( 
                std::format(
                    "Could not open file: {}",
                    filePath.string()
                ),
                ErrorCategory::FileIO,
                ErrorSeverity::Fatal
            )
        );
    }

    Logger::trace( 
        "Opening file",
        pathAttr
    );

    auto data = FileData( filePath );

    data.addLineOffset( file.tellg() );

    std::string line;
    while ( std::getline( file, line ) ) 
    {
        data.addLineOffset( file.tellg() );
    }

    Logger::trace( 
        std::format(
            "Collected {} line(s) for file",
            data.getLinesCollected()
        ),
        pathAttr
    );

    m_files.emplace( data.getFileId(), std::move( data ) );
    m_pathToId.emplace( filePath, data.getFileId() );

    Logger::trace( 
        std::format(
            "Assigned file ID {} to {}",
            data.getFileId(),
            filePath.string()
        )
    );

    return data.getFileId();
}

std::expected<std::string, Diagnostic> SourceManager::getLine( FileId fileId, std::size_t lineNumber ) const
{
    auto it = m_files.find( fileId );
    if ( it == m_files.end() ) 
    {
        Logger::trace(
            std::format(
                "Attempted to fetch line {} from invalid file ID",
                lineNumber
            ),
            std::to_array<Attribute>({
                { "fileId", std::to_string( fileId ) }
            })
        );

        return std::unexpected( 
            Diagnostic(
                "Invalid file ID",
                ErrorCategory::Linking,
                ErrorSeverity::Fatal 
            )
        );
    }

    const FileData& data = it->second;

    const std::array pathAttr { 
        Attribute{ "Path", std::format( "'{}'", data.getFilePath().string() ) } 
    };

    if ( lineNumber == 0 || lineNumber > data.getLinesCollected() )
    {
        Logger::trace( 
            std::format(
                "Line number {} out of range for file",
                lineNumber
            ),
            pathAttr
        );

        return std::unexpected( 
            Diagnostic(
                "Line number out of range",
                ErrorCategory::FileIO,
                ErrorSeverity::Fatal 
            )
        );
    }

    Logger::trace( 
        std::format(
            "Fetching line {} from file",
            lineNumber 
        ),
        pathAttr
    );

    std::ifstream file( data.getFilePath(), std::ios::binary );

    if ( !file.is_open() ) 
    {
        Logger::trace( 
            "Failed to reopen file",
            pathAttr
        );
        
        return std::unexpected(
            Diagnostic(
                "Could not reopen file",
                ErrorCategory::FileIO,
                ErrorSeverity::Fatal
            )
        );
    }

    file.clear();
    file.seekg( data.getStreamPosition( lineNumber ) );

    std::string line;
    std::getline( file, line );

    Logger::trace(
        std::format(
            "Retrieved line {} from file",
            lineNumber
        ),
        std::to_array<Attribute>({
            { "Path", std::format( "'{}'", data.getFilePath().string() ) },
            { "Length", std::to_string( line.size() ) }
        })
    );

    return line;
}
