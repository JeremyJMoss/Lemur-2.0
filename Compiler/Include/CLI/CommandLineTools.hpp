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

struct BuildConfig {
    bool optimize = false;
    std::string outputPath;
    bool emitAST = false;
    std::string sourcePath;
    bool loggingEnabled = false;
    LogLevel logLevel = LogLevel::INFO;
    std::string logPath;
};

struct InitConfig {
    std::string name;
};

struct CLIConfig {
    Command command;

    std::variant<
        BuildConfig,
        InitConfig
    > data;
};

class CommandLineTools {
    public:
        std::expected<CLIConfig, CommandLineError> parse( int argc, char* argv[] );
    private:
        Command parseCommand( std::string_view str );
        void printHelp( const std::string& command, const std::string& issue );
};