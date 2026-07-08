#pragma once

/* === Imports === */

#include <filesystem>
#include <unordered_map>
#include <optional>
#include <string>
#include "Modules/ModuleTable.hpp"

namespace fs = std::filesystem;

using FileId = std::size_t;
using ModuleId = std::size_t;

/* === Forward Declares === */

class SourceManager;
class ErrorReporter;

/* === Module Resolver === */

class ModuleResolver {
    public:
        ModuleResolver(
            SourceManager& srcManager,
            ErrorReporter& errReporter
        ) : 
        m_srcManager( srcManager ),
        m_errReporter( errReporter ) {}

        void populate( const fs::path& sourcePath, ModuleTable& moduleTable );

    private:
        SourceManager& m_srcManager;
        ErrorReporter& m_errReporter;
};
