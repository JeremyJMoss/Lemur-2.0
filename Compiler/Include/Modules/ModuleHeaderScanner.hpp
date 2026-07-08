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

/* === Module Header Scanner === */

class ModuleHeaderScanner {
    public:
        ModuleHeaderScanner(
            SourceManager& srcManager,
            ErrorReporter& errReporter
        ) : 
        m_srcManager( srcManager ),
        m_errReporter( errReporter ) {}

        ModuleTable scan( const fs::path& sourcePath );

    private:
        SourceManager& m_srcManager;
        ErrorReporter& m_errReporter;
};
