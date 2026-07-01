#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "Utils/Logger.hpp"
#include "Driver/Driver.hpp"
#include "Utils/Output.hpp"
#include "CLI/CommandLineTools.hpp"

namespace fs = std::filesystem;

namespace chrono = std::chrono;

int main( int argc, char* argv[] )
{
    auto start = chrono::high_resolution_clock::now();

    CommandLineTools cli;

    CLIConfig cliConfig = cli.parse( argc, argv );

    Driver compiler = Driver();

    compiler.compileProgram();

    auto end = chrono::high_resolution_clock::now();
    auto duration = duration_cast<chrono::microseconds>( end - start );

    std::cout << "Entire Program Execution time: " << duration.count() << " µs\n";

    return EXIT_SUCCESS;
}