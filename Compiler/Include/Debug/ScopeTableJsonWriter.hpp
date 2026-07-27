#pragma once

/* === Dependencies === */

#include <string>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

/* === Forward Declarations === */

class CompilerContext;

/* === Scope Table Json Writer === */

class ScopeTableJsonWriter
{
    public:
        void write( const CompilerContext& ctx, const fs::path& outputPath, const std::string_view moduleName );

    private:
        std::ofstream* m_out;
};