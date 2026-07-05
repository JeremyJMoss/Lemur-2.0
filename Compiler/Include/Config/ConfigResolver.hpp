#pragma once
#include "Config/Config.hpp"

struct ConfigResolver {
    public:
        static void mergeConfigurations( const BuildCLIConfig& bConfig, CompilerConfig& cConfig );
};