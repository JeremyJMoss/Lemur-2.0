#include "CLI/CommandLineTools.hpp"
#include "Utils/Logger.hpp"
#include <filesystem>

namespace fs = std::filesystem;

std::expected<CLIConfig, CLIStatus> CommandLineTools::parse( int argc, char* argv[] ) 
{
    CLIConfig config;

    if (argc > 1 && std::string_view(argv[1]) == "--help") {
        config.showHelp = true;
        return config;
    }

    if (argc < 2)
    {
        return std::unexpected(CLIStatus::MissingCommand);
    }

    config.command = parseCommand(argv[1]);

    switch (config.command) {
        case Command::Unknown: {
            return std::unexpected(CLIStatus::UnknownCommand);
        }
        case Command::Init: {
            if ( argc < 3 ) {
                return std::unexpected(CLIStatus::MissingArgument);
            }

            if ( argc > 3 && argv[3] == std::string_view("--help") ) {
                config.showHelp = true;
                return config;
            }

            InitCLIConfig initConfig;

            initConfig.name = argv[2];

            config.data = initConfig;

            break;
        }
        case Command::Build: {
            if ( argc > 2 && argv[2] == std::string_view("--help") ) {
                config.showHelp = true;
                return config;
            }

            BuildCLIConfig buildConfig;

            for ( int i = 2; i < argc; ++i ) 
            {
                std::string_view arg = argv[ i ];

                if ( arg == "--log" && i + 1 < argc ) 
                {
                    buildConfig.loggingEnabled = true;
                    buildConfig.logLevel = Logger::parseLogLevel( argv[ ++i ] );
                    continue;
                }

                if ( arg == "--log-file" && i + 1 < argc )
                {
                    buildConfig.logPath = fs::absolute(argv[ ++i ]);
                    continue;
                }

                if ( ( arg == "--src" || arg == "-s" ) && i + 1 < argc ) {
                    fs::path sourcePath = fs::absolute(argv[ ++i ]);
                    if ( !isValidSourcePath(sourcePath)) {
                        return std::unexpected(CLIStatus::InvalidSourcePath);
                    }
                    buildConfig.sourcePath = sourcePath;
                    continue;
                }

                if ( ( arg == "--out" || arg == "-o" ) && i + 1 < argc ) {
                    fs::path outputPath = fs::absolute(argv[ ++i ]);
                    buildConfig.outputPath = outputPath;
                    continue;
                }

                if ( ( arg == "--entry" || arg == "-e" ) && i + 1 < argc ) {
                    buildConfig.entryModule = argv[ ++i ];
                    continue;
                }

                if ( arg == "--emit-ast" ) {
                    buildConfig.emitAST = true;
                    continue;
                }

                if ( arg == "--optimize" ) {
                    buildConfig.optimize = true;
                }
            }

            config.data = buildConfig;
            break;
        }
    }

    return config;
}

Command CommandLineTools::parseCommand( std::string_view str )
{
    if (str == "build") return Command::Build;
    if (str == "init")  return Command::Init;

    return Command::Unknown;
}

void CommandLineTools::printIssue( CLIStatus status ) 
{
    std::cout << '\n';

    std::cout << "Error: ";

    switch ( status ) {
        case CLIStatus::MissingArgument: {
            std::cout << "Missing argument to command.\n";
            break;
        }
        case CLIStatus::MissingCommand: {
            std::cout << "Missing command.\n";
            printHelp( Command::Unknown );
            return;
        }
        case CLIStatus::InvalidOption: {
            std::cout << "Invalid option to command.\n";
            break;
        }
        case CLIStatus::InvalidValue: {
            std::cout << "Invalid value provided.\n";
            break;
        }
        case CLIStatus::TooManyArguments: {
            std::cout << "Too many arguments passed into command.\n";
            break;
        }
        case CLIStatus::UnknownCommand: {
            std::cout << "Unknown command passed.\n";
            printHelp( Command::Unknown );
            return;
        }
        case CLIStatus::InvalidSourcePath: {
            std::cout << "Invalid source path passed into command.\n";
            break;
        }
    }
}

bool CommandLineTools::isValidSourcePath( fs::path path ) {
    std::error_code ec;

    return fs::exists(path, ec) && fs::is_directory(path, ec);
}

void CommandLineTools::printHelp( Command command ) 
{
    std::cout << '\n';

    std::cout << "Lemur Compiler Usage:\n\n";

    switch (command) {
        case Command::Unknown: {
            std::cout << "  lemur build [options]\n";
            std::cout << "  lemur init <name>\n\n";

            std::cout << "Lemur Build Options:\n";
            std::cout << "  --src -s <path>        Set source directory\n";
            std::cout << "  --output -o <path>     Set output directory\n";
            std::cout << "  --entry -e <path>      Set entry module name\n";
            std::cout << "  --optimize             Set whether output should be optimized for production use\n";
            std::cout << "  --emitAST              Set whether Abstract Syntax Tree output should be emitted as json file in output directory\n";
            std::cout << "  --log <level>          Set log level (ERROR, WARN, INFO, DEBUG, TRACE)\n";
            std::cout << "  --log-file <path>      Set log output file\n\n";

            std::cout << "Examples:\n";
            std::cout << "  lemur build app --log DEBUG\n";
            std::cout << "  lemur init my_project\n\n";

            std::cout << "Commands:\n";
            std::cout << "  build   Compile a module\n";
            std::cout << "  init    Create a new project\n\n";

            break;
        }
        case Command::Init: {
            std::cout << "  lemur init <name> - Creates a new project\n\n";

            std::cout << "Examples:\n";
            std::cout << "  lemur init my_project\n\n";
            break;
        }
        case Command::Build: {
            std::cout << "  lemur build [options] - Compiles a module\n\n";

            std::cout << "Lemur Build Options:\n";
            std::cout << "  --src -s <path>        Set source directory\n";
            std::cout << "  --output -o <path>     Set output directory\n";
            std::cout << "  --entry -e <path>      Set entry module name\n";
            std::cout << "  --optimize             Set whether output should be optimized for production use\n";
            std::cout << "  --emitAST              Set whether Abstract Syntax Tree output should be emitted as json file in output directory\n";
            std::cout << "  --log <level>          Set log level (ERROR, WARN, INFO, DEBUG, TRACE)\n";
            std::cout << "  --log-file <path>      Set log output file\n\n";
            
            std::cout << "Examples:\n";
            std::cout << "  lemur build app --log DEBUG\n\n";
            break;
        }
    }

    std::cout << '\n';
}