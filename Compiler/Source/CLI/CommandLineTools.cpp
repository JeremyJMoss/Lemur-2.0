#include "CLI/CommandLineTools.hpp"
#include "Utils/Logger.hpp"

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

            InitConfig initConfig;

            initConfig.name = argv[2];

            config.data = initConfig;

            break;
        }
        case Command::Build: {
            if (argc < 3) {
                return std::unexpected(CLIStatus::MissingArgument);
            }

            if ( argc > 3 && argv[3] == std::string_view("--help") ) {
                config.showHelp = true;
                return config;
            }

            BuildConfig buildConfig;

            for ( int i = 3; i < argc; ++i ) 
            {
                std::string_view arg = argv[ i ];

                if ( arg == "--log" && i + 1 < argc ) 
                {
                    buildConfig.loggingEnabled = true;
                    buildConfig.logLevel = Logger::parseLogLevel( argv[ ++i ] );
                }

                if ( arg == "--log-file" && i + 1 < argc )
                {
                    buildConfig.logPath = argv[ ++i ];
                }

                if ( ( arg == "--src" || arg == "-s" ) && i + 1 < argc ) {
                    buildConfig.sourcePath = argv[ ++i ];
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
            std::cout << "Missing argument to command.";
            break;
        }
        case CLIStatus::MissingCommand: {
            std::cout << "Missing command\n";
            printHelp( Command::Unknown );
            return;
        }
        case CLIStatus::InvalidOption: {
            std::cout << "Invalid option to command";
            break;
        }
        case CLIStatus::InvalidValue: {
            std::cout << "Invalid value provided";
            break;
        }
        case CLIStatus::TooManyArguments: {
            std::cout << "Too many arguments passed into command";
            break;
        }
        case CLIStatus::UnknownCommand: {
            std::cout << "Unknown command passed\n";
            printHelp( Command::Unknown );
            return;
        }
    }
}

void CommandLineTools::printHelp( Command command ) 
{
    std::cout << '\n';

    std::cout << "Lemur Compiler Usage:\n\n";

    switch (command) {
        case Command::Unknown: {
            std::cout << "  lemur build <module> [options]\n";
            std::cout << "  lemur init <name>\n";

            std::cout << "Options:\n";
            std::cout << "  --src <path>        Set source directory\n";
            std::cout << "  --log <level>       Set log level (ERROR, WARN, INFO, DEBUG, TRACE)\n";
            std::cout << "  --log-file <path>   Set log output file\n\n";

            std::cout << "Examples:\n";
            std::cout << "  lemur build app --log DEBUG\n";
            std::cout << "  lemur init my_project\n\n";

            std::cout << "Commands:\n";
            std::cout << "  build   Compile a module\n";
            std::cout << "  init    Create a new project\n";

            break;
        }
        case Command::Init: {
            std::cout << "  lemur init <name>\n\n";
            std::cout << "Examples:\n";
            std::cout << "  lemur init my_project\n\n";
        }
        case Command::Build: {
            std::cout << "  lemur build <module> [options]\n\n";
            
            std::cout << "Examples:\n";
            std::cout << "  lemur build app --log DEBUG\n\n";

            std::cout << "Commands:\n";
            std::cout << "  build   Compile a module";
        }
    }

    std::cout << '\n';
}