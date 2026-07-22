#pragma once

/* === Dependencies ===*/

#include <string>
#include <filesystem>
#include <unordered_map>
#include <expected>

namespace fs = std::filesystem;

/* === Imports === */

#include "SourceControl/FileData.hpp"
#include "Errors/Errors.hpp"
#include "Core/Ids.hpp"

/* === Source Manager === */

class SourceManager 
{
    public:
        std::expected<FileId, Diagnostic> addFile( const fs::path& filePath );

        std::expected<std::string, Diagnostic> getLine( FileId fileId, std::size_t lineNumber ) const;

        fs::path getFilePath( FileId fileId ) const;

    private:
        std::unordered_map<FileId, FileData> m_files;
        std::unordered_map<fs::path, FileId> m_pathToId;
};