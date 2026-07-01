#include "CLI/CommandLineTools.hpp"
#include "Utils/Logger.hpp"

std::expected<CLIConfig, CommandLineError> CommandLineTools::parse( int argc, char* argv[] ) 
{
    CLIConfig config;

    if (argc < 2)
    {
        printHelp("", "No command provided");
        return std::unexpected(CommandLineError("No command provided"));
    }

    config.command = parseCommand(argv[1]);

    switch (config.command) {
        case Command::Unknown: {
            printHelp(argv[1], "Unknown command");
            return std::unexpected(CommandLineError("Unknown command"));
        }
        case Command::Init: {
            if ( argc > 3 ) {
                printHelp(argv[1], "Too many arguments passed to command");
                return std::unexpected(CommandLineError("Too many arguments passed to command"));
            }
            break;
        }
        case Command::Build: {
            if (argc < 3) {
                printHelp(argv[1], "Missing required module entry point name");
                return std::unexpected(CommandLineError("Missing required module entry point name"));
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

void CommandLineTools::printHelp( const std::string& command, const std::string& issue ) {
    std::cout << "\n";
    
    if (!command.empty())
    {
        std::cout << "Error: " << issue << "\n";
        std::cout << "Unknown command: " << command << "\n\n";
    }

    std::cout << "Lemur Compiler Usage:\n\n";

    std::cout << "  lemur build <module> [options]\n";
    std::cout << "  lemur init [name]\n";

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

    std::cout << std::endl;
}