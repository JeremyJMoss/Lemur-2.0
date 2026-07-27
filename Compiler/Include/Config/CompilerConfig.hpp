#pragma once

/* === Dependencies ===*/

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

/* === Compiler Config === */

struct CompilerConfig {
    bool optimize = false;
    bool dumpAST = false;
    bool dumpScopes = false;
    bool dumpSymbols = false;
    bool dumpTypes = false;
    bool dumpOverloads = false;
    bool dumpNodeSemantics = false;
    fs::path sourcePath;
    fs::path outputPath;
    std::string entryModule;
};