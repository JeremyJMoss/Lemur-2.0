#pragma once

/* === Imports === */
#include <string>
#include <expected>
#include <filesystem>
#include "Errors/Errors.hpp"
#include "Config/Config.hpp"

namespace fs = std::filesystem;

/* === Enum Declaration === */

enum class CLIStatus {
    MissingCommand,
    UnknownCommand,
    MissingArgument,
    TooManyArguments,
    InvalidOption,
    InvalidValue,
    InvalidSourcePath
};

/* === Command Line Tools === */

class CommandLineTools {
    public:
        std::expected<CLIConfig, CLIStatus> parse( int argc, char* argv[] );
        static void printHelp( Command command );
        static void printIssue( CLIStatus status );
    private:
        Command parseCommand( std::string_view str );
        bool isValidSourcePath( fs::path path );
        
};