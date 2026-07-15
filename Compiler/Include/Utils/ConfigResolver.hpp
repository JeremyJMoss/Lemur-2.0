#pragma once

/* === Forward Declarations === */

struct BuildCLIConfig;
struct CompilerConfig;

/* === Config Resolver === */

struct ConfigResolver {
    public:
        static void mergeConfigurations( const BuildCLIConfig& bConfig, CompilerConfig& cConfig );
};