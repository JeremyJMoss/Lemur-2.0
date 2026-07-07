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

        const std::string getLine( FileId fileId, std::size_t lineNumber ) const;

        fs::path getFilePath( FileId fileId ) const
        {
            return m_files.at( fileId ).getFilePath().string();
        }

        void setModuleName( FileId id, std::string_view moduleName );

    private:
        std::unordered_map<FileId, FileData> m_files;
        std::unordered_map<fs::path, FileId> m_pathToId;
};