#include "SourceControl/SourceManager.hpp"
#include "Utils/Logger.hpp"
#include <array>
#include <string>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace fs = std::filesystem;

using FileId = std::size_t;

FileId SourceManager::addFile( const fs::path& filePath )
{
    const auto pathStr = "'" + filePath.string() + "'";

    const std::array pathAttr { 
        Attribute{ "Path", pathStr } 
    };

    auto it = pathToId.find( filePath );

    if ( it != pathToId.end() ) 
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
        throw std::runtime_error( "Could not open file: " + filePath.string() );
    }

    Logger::trace( 
        "Opening file",
        pathAttr
    );

    auto data = FileData(filePath);

    data.addLineOffset( file.tellg() );

    std::string line;
    while ( std::getline( file, line ) ) 
    {
        data.addLineOffset( file.tellg() );
    }

    Logger::trace( 
        "Collected " + 
        std::to_string( data.getLinesCollected() ) +
        " line(s) for file",
        pathAttr
    );

    files.emplace( data.getFileId(), std::move( data ) );
    pathToId.emplace( filePath, data.getFileId() );

    Logger::trace( 
        "Assigned file ID " + 
        std::to_string( data.getFileId() ) + 
        " to " + filePath.string() 
    );

    return data.getFileId();
}

std::string_view SourceManager::getLine( FileId fileId, std::size_t lineNumber ) const
{
    auto it = files.find( fileId );
    if ( it == files.end() ) 
    {
        Logger::trace( 
            "Attempted to fetch line " + 
            std::to_string( lineNumber ) +
            " from invalid file ID",
            std::to_array<Attribute>({
                { "fileId", std::to_string( fileId ) }
            })
        );

        throw std::runtime_error( "Invalid file ID" );
    }

    const FileData& data = it->second;

    const std::array pathAttr { 
        Attribute{ "Path", "'" + data.getFilePath().string() + "'"} 
    };

    if ( lineNumber == 0 || lineNumber > data.getLinesCollected() )
    {
        Logger::trace( 
            "Line number " + 
            std::to_string( lineNumber ) +
            " out of range for file",
            pathAttr
        );

        throw std::out_of_range( "Line number out of range" );
    }

    Logger::trace( 
        "Fetching line " + 
        std::to_string( lineNumber ) +
        " from file",
        pathAttr
    );

    std::ifstream file( data.getFilePath(), std::ios::binary );

    if ( !file.is_open() ) 
    {
        Logger::trace( 
            "Failed to reopen file",
            pathAttr
        );
        
        throw std::runtime_error( "Could not reopen file" );
    }

    file.clear();
    file.seekg( data.getStreamPosition( lineNumber ) );

    std::string line;
    std::getline( file, line );

    Logger::trace( 
        "Retrieved line " + 
        std::to_string( lineNumber ) +
        " from file",
        std::to_array<Attribute>({
            { "Path", "'" + data.getFilePath().string() + "'" },
            { "Length", std::to_string( line.size() ) }
        })
    );

    return line;
}
