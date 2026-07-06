#pragma once

/* === Imports === */

#include <string>
#include <filesystem>
#include <unordered_map>
#include "SourceControl/FileData.hpp"

namespace fs = std::filesystem;

using FileId = size_t;

/* === Source Manager === */

class SourceManager 
{
    public:
        FileId addFile( const fs::path& filePath );

        std::string_view getLine( FileId fileId, std::size_t lineNumber ) const;

        fs::path getFilePath( FileId fileId ) const
        {
            return files.at( fileId ).getFilePath().string();
        }

    private:
        std::unordered_map<FileId, FileData> files;
        std::unordered_map<fs::path, FileId> pathToId;
};