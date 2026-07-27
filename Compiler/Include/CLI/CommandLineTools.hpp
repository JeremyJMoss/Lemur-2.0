#pragma once

/* === Dependencies === */

#include <string>
#include <expected>
#include <filesystem>

namespace fs = std::filesystem;

/* === Imports === */

#include "Errors/Errors.hpp"
#include "CLI/CLIConfig.hpp"

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
        
        static constexpr std::string_view s_generalHelp = 
            "\nLemur Compiler Usage:\n"
            "    lemur build [options]\n"
            "    lemur init <name>\n\n"
            "Lemur Build Options:\n"
            "    --src -s <path>        Set source directory\n"
            "    --output -o <path>     Set output directory\n"
            "    --entry -e <path>      Set entry module name\n"
            "    --optimize             Enable production optimizations\n"
            "    --dump-ast             Emit the AST as JSON\n"
            "    --dump-scopes          Emit the Scope table as JSON\n"
            "    --dump-symbols         Emit the Symbol table as JSON\n"
            "    --dump-types           Emit the Type table as JSON\n"
            "    --dump-overloads       Emit the Function overload table as JSON\n"
            "    --dump-semantics       Emit the Node relationship table as JSON\n"
            "    --log <level>          Set log level (ERROR, WARN, INFO, DEBUG, TRACE)\n"
            "    --log-file <path>      Set log output file\n\n"
            "Examples:\n"
            "    lemur build --log DEBUG\n"
            "    lemur init my_project\n\n"
            "Commands:\n"
            "    build   Compile a project\n"
            "    init    Create a new project\n\n";

        static constexpr std::string_view s_buildHelp =
            "\nLemur Compiler Usage:\n"
            "    lemur build [options] - Compiles a module\n\n"
            "Lemur Build Options:\n"
            "    --src -s <path>        Set source directory\n"
            "    --output -o <path>     Set output directory\n"
            "    --entry -e <path>      Set entry module name\n"
            "    --optimize             Enable production optimizations\n"
            "    --dump-ast             Emit the AST as JSON\n"
            "    --dump-scopes          Emit the Scope table as JSON\n"
            "    --dump-symbols         Emit the Symbol table as JSON\n"
            "    --dump-types           Emit the Type table as JSON\n"
            "    --dump-overloads       Emit the Function overload table as JSON\n"
            "    --dump-semantics       Emit the Node relationship table as JSON\n"
            "    --log <level>          Set log level (ERROR, WARN, INFO, DEBUG, TRACE)\n"
            "    --log-file <path>      Set log output file\n\n"
            "Examples:\n"
            "    lemur build --log DEBUG\n\n";
    
        static constexpr std::string_view s_initHelp =
            "\nLemur Compiler Usage:\n"
            "    lemur init <name> - Creates a new project\n\n"
            "Examples:\n"
            "    lemur init my_project\n\n";
};