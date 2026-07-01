#pragma once;

/* === Imports === */

#include <filesystem>
#include <unordered_map>

namespace fs = std::filesystem;

/* === Forward Declares === */

class SourceManager;
class ErrorReporter;

/* === ModuleResolver === */

class ModuleResolver {
    public:
        ModuleResolver(
            fs::path root,
            SourceManager& srcManager,
            ErrorReporter& errReporter
        ) : m_rootFolder( std::move( root ) ),
        m_srcManager( srcManager ),
        m_errReporter( errReporter ) {}

        // scans the project and finds all modules starting at m_root
        void scan();

    private:
        fs::path m_rootFolder;
        SourceManager& m_srcManager;
        ErrorReporter& m_errReporter;
        std::unordered_map<std::string, fs::path> moduleIndex;
};
