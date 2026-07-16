#pragma once

/* === Dependencies ===*/

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

/* === Compiler Config === */

struct CompilerConfig {
    bool optimize = false;
    bool emitAST = false;
    fs::path sourcePath;
    fs::path outputPath;
    std::string entryModule;
};