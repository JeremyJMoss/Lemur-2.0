#pragma once

/* === Dependencies ===*/

#include <optional>
#include <string>
#include <filesystem>
#include <variant>

namespace fs = std::filesystem;

/* === Imports === */

#include "Logging/Logger.hpp"
#include "Config/Command.hpp"

/* === Build CLI Config === */

struct BuildCLIConfig {
    std::optional<bool> optimize;
    std::optional<bool> loggingEnabled;
    std::optional<bool> dumpAST;
    std::optional<bool> dumpScopes;
    std::optional<bool> dumpSymbols;
    std::optional<bool> dumpTypes;
    std::optional<bool> dumpOverloads;
    std::optional<bool> dumpNodeSemantics;
    std::optional<fs::path> sourcePath;
    std::optional<fs::path> outputPath;
    std::optional<LogLevel> logLevel;
    std::optional<fs::path> logPath;
    std::optional<std::string> entryModule;
};

/* === Init CLI Config === */

struct InitCLIConfig {
    std::string name;
};

/* === CLI Config === */

struct CLIConfig {
    Command command = Command::Unknown;

    bool showHelp = false;

    std::variant<
        BuildCLIConfig,
        InitCLIConfig
    > data;
};