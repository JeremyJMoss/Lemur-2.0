#pragma once

/* === Dependencies ===*/

#include <optional>
#include <string>
#include <filesystem>
#include <variant>

namespace fs = std::filesystem;

/* === Imports === */

#include "Utils/Logger.hpp"
#include "Config/Command.hpp"

/* === Build CLI Config === */

struct BuildCLIConfig {
    std::optional<bool> optimize;
    std::optional<bool> loggingEnabled;
    std::optional<bool> emitAST;
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