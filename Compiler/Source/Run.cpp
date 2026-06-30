#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "Utils/Logger.hpp"
#include "LanguageConfig.hpp"
#include "Driver/Driver.hpp"
#include "Utils/Output.hpp"

namespace fs = std::filesystem;

namespace chrono = std::chrono;

std::string getCLIConfig( int argc, char* argv[] ) 
{
    LogLevel level = LogLevel::INFO;
    Logger::setFile( Lemur::LOG_FILE_PATH );
    std::string filePath = "";

    for ( int i = 1; i < argc; ++i ) 
    {
        std::string arg = argv[ i ];

        if ( arg == "--log" && i + 1 < argc ) 
        {
            Logger::setShouldLog( true );
            level = Logger::parseLogLevel( argv[ ++i ] );
        }

        if ( arg == "--quiet" || arg == "-q" )
        {
            Output::setQuiet( true );
        }

        if ( ( arg == "--src" || arg == "-s" ) && i + 1 < argc ) {
            filePath = argv[ ++i ];
        }
    }

    Logger::setLevel( level );
    return filePath;
}

int main( int argc, char* argv[] )
{
    auto start = chrono::high_resolution_clock::now();

    std::string filePath = getCLIConfig( argc, argv );

    if (filePath.empty()) {
        std::cerr << "Did not include src file path using --src of -s flag";
        return EXIT_FAILURE;
    }

    Driver compiler = Driver();

    compiler.compileProgram( filePath );

    auto end = chrono::high_resolution_clock::now();
    auto duration = duration_cast<chrono::microseconds>( end - start );

    std::cout << "Entire Program Execution time: " << duration.count() << " µs\n";

    return EXIT_SUCCESS;
}