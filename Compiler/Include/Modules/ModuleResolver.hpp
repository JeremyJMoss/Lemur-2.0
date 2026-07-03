#pragma once;

/* === Imports === */

#include <filesystem>
#include <unordered_map>

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

        // scans the project and finds all modules starting at root folder
        void buildModuleIndex( fs::path sourcePath );

    private:
        SourceManager& m_srcManager;
        ErrorReporter& m_errReporter;
        std::unordered_map<std::string, FileId> moduleIndex;
};
