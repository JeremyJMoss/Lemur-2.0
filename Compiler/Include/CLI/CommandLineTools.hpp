#pragma once

#include <string>
#include <variant>
#include <expected>
#include "Errors/Errors.hpp"

enum class Command {
    Build,
    Init,
    Unknown
};

enum class CLIStatus {
    MissingCommand,
    UnknownCommand,
    MissingArgument,
    TooManyArguments,
    InvalidOption,
    InvalidValue,
};

struct BuildConfig {
    bool optimize = false;
    bool loggingEnabled = false;
    bool emitAST = false;
    std::string outputPath;
    std::string sourcePath;
    LogLevel logLevel = LogLevel::INFO;
    std::string logPath;
    std::string entryModule;
};

struct InitConfig {
    std::string name;
};

struct CLIConfig {
    Command command = Command::Unknown;

    bool showHelp = false;

    std::variant<
        BuildConfig,
        InitConfig
    > data;
};

class CommandLineTools {
    public:
        std::expected<CLIConfig, CLIStatus> parse( int argc, char* argv[] );
        static void printHelp( Command command );
        static void printIssue( CLIStatus status );
    private:
        Command parseCommand( std::string_view str );
        
};