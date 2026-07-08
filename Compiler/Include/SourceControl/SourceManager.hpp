#pragma once

/* === Imports === */

#include <string>
#include <filesystem>
#include <unordered_map>
#include <expected>
#include "SourceControl/FileData.hpp"
#include "Errors/Errors.hpp"
#include "Modules/ModuleTable.hpp"

namespace fs = std::filesystem;

using FileId = size_t;

/* === Source Manager === */

class SourceManager 
{
    public:
        std::expected<FileId, Diagnostic> addFile( const fs::path& filePath );

        std::expected<std::string, Diagnostic> getLine( FileId fileId, std::size_t lineNumber ) const;

        fs::path getFilePath( FileId fileId ) const
        {
            return m_files.at( fileId ).getFilePath().string();
        }

        void setModuleInfo( const ModuleInfo& info );

    private:
        std::unordered_map<FileId, FileData> m_files;
        std::unordered_map<fs::path, FileId> m_pathToId;
};