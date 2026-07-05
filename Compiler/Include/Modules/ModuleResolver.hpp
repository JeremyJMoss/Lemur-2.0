#pragma once

/* === Imports === */

#include <filesystem>
#include <unordered_map>
#include <optional>

namespace fs = std::filesystem;

using FileId = std::size_t;

/* === Forward Declares === */

class SourceManager;
class ErrorReporter;

/* === ModuleResolver === */

class ModuleResolver {
    public:
        ModuleResolver(
            SourceManager& srcManager,
            ErrorReporter& errReporter
        ) : 
        m_srcManager( srcManager ),
        m_errReporter( errReporter ) {}

        void buildModuleIndex( const fs::path& sourcePath );

        std::optional<FileId> resolveModuleFileId( const std::string& moduleName ) const;

    private:
        SourceManager& m_srcManager;
        ErrorReporter& m_errReporter;
        std::unordered_map<std::string, FileId> m_moduleIndex;
};
