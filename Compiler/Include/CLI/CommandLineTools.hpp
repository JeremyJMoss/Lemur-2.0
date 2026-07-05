#pragma once

#include <string>
#include <expected>
#include "Errors/Errors.hpp"
#include "Config/Config.hpp"

enum class CLIStatus {
    MissingCommand,
    UnknownCommand,
    MissingArgument,
    TooManyArguments,
    InvalidOption,
    InvalidValue,
    InvalidSourcePath
};

class CommandLineTools {
    public:
        std::expected<CLIConfig, CLIStatus> parse( int argc, char* argv[] );
        static void printHelp( Command command );
        static void printIssue( CLIStatus status );
    private:
        Command parseCommand( std::string_view str );
        bool isValidSourcePath( fs::path path );
        
};