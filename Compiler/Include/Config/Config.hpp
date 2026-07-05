#pragma once

#include <optional>
#include <variant>
#include <filesystem>
#include <vector>
#include "Utils/Logger.hpp"

namespace fs = std::filesystem;

enum class Command {
    Build,
    Init,
    Unknown
};

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

struct InitCLIConfig {
    std::string name;
};

struct CLIConfig {
    Command command = Command::Unknown;

    bool showHelp = false;

    std::variant<
        BuildCLIConfig,
        InitCLIConfig
    > data;
};

struct CompilerConfig {
    bool optimize = false;
    bool emitAST = false;
    fs::path sourcePath;
    fs::path outputPath;
    std::string entryModule;
};