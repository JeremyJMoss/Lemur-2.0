#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "Utils/Logger.hpp"
#include "Driver/Driver.hpp"
#include "Utils/Output.hpp"
#include "Utils/Logger.hpp";
#include "CLI/CommandLineTools.hpp"

namespace fs = std::filesystem;

namespace chrono = std::chrono;

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
            build(cliConfig);
            break;
        }
        case Command::Init: {
            init(cliConfig);
            break;
        }
        case Command::Unknown: {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

void build( const CLIConfig& config ) {
    auto start = chrono::high_resolution_clock::now();

    BuildConfig buildConfig = std::get<BuildConfig>(config.data);

    if (buildConfig.loggingEnabled) {
        Logger::setShouldLog(true);
        Logger::setLevel(buildConfig.logLevel);
    }

    if (buildConfig.logPath.size() > 0) {
        Logger::setFile(buildConfig.logPath);
    }

    CompilerConfig compConfig;

    compConfig.sourcePath = buildConfig.sourcePath;
    compConfig.entryModule = buildConfig.entryModule;

    Driver compilerDriver = Driver( compConfig );

    compilerDriver.compileProgram();

    auto end = chrono::high_resolution_clock::now();
    auto duration = duration_cast<chrono::microseconds>( end - start );

    std::cout << "Entire Program Execution time: " << duration.count() << " µs\n";
}

void init( const CLIConfig& config ) {
    std::cout << "Initialising Project: ";

    InitConfig initConfig = std::get<InitConfig>(config.data);

    std::cout << initConfig.name << std::endl;
}

