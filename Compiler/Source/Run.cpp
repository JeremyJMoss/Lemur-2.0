#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "Utils/Logger.hpp"
#include "Driver/Driver.hpp"
#include "Utils/Output.hpp"
#include "Utils/Logger.hpp"
#include "CLI/CommandLineTools.hpp"
#include "Config/Config.hpp"
#include "Utils/TomlConfigHandler.hpp"
#include "Config/ConfigResolver.hpp"

namespace fs = std::filesystem;

namespace chrono = std::chrono;

void build( const CLIConfig& config ) {
    auto start = chrono::high_resolution_clock::now();

    BuildCLIConfig buildConfig = std::get<BuildCLIConfig>(config.data);

    // configure logging setup
    Logger::setShouldLog(buildConfig.loggingEnabled.value_or(false));
    Logger::setLevel(buildConfig.logLevel.value_or(LogLevel::INFO));
    Logger::setFile(buildConfig.logPath.value_or("./logs/compiler.log"));

    TomlConfigHandler tomlHandler;

    auto maybeCompConfig = tomlHandler.parseOrFail();

    if (!maybeCompConfig) {
        throw maybeCompConfig.error();
    }

    CompilerConfig compConfig = maybeCompConfig.value();

    ConfigResolver::mergeConfigurations( buildConfig, compConfig );

    Driver compilerDriver = Driver( compConfig );

    compilerDriver.compileProgram();

    auto end = chrono::high_resolution_clock::now();
    auto duration = duration_cast<chrono::microseconds>( end - start );

    std::cout << "Entire Program Execution time: " << duration.count() << " µs\n";
}

bool init( const CLIConfig& config ) {
    InitCLIConfig initConfig = std::get<InitCLIConfig>(config.data);

    Output::info( "Initialising Project: " + std::string( initConfig.name ) );

    TomlConfigHandler tomlHandler;

    if (!tomlHandler.createTomlFile( initConfig.name )) {
        return false;
    }

    fs::path buildFolder = "build";
    if (!fs::create_directories(buildFolder)) {
        return false;
    }

    fs::path sourceFolder = "src";
    if (!fs::create_directories(sourceFolder)) {
        return false;
    }

    fs::path filePath = sourceFolder / "test.lmur";

    std::ofstream file(filePath);

    if (!file)
    {
        return false;
    }

    file << "module app;\n\n";
    file << "entry fn main(): int {\n";
    file << "    return 0;\n";
    file << "}";

    return true;
}

int main( int argc, char* argv[] )
{
    CommandLineTools cli;

    auto parseResult = cli.parse( argc, argv );

    if ( !parseResult ) {
        CommandLineTools::printIssue(parseResult.error());
        return EXIT_FAILURE;
    }

    CLIConfig cliConfig = parseResult.value();

    if (cliConfig.showHelp) {
        CommandLineTools::printHelp( cliConfig.command );
        return EXIT_SUCCESS;
    }

    switch (cliConfig.command) {
        case Command::Build: {
            try {
                build(cliConfig);
            } catch (const std::exception& e) {
                Output::error(e.what());
                return EXIT_FAILURE;
            }
            break;
        }
        case Command::Init: {
            bool success = init(cliConfig);
            if (!success) {
                Output::error( "Failed to create lemur.toml file in root directory" );
                return EXIT_FAILURE;
            }
            Output::success( "Successfully create lemur.toml file in root directory" );
            break;
        }
        case Command::Unknown: {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

