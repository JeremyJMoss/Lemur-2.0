#pragma once

#include <expected>
#include <filesystem>
#include "Config/CompilerConfig.hpp"
#include "Errors/Errors.hpp"

namespace fs = std::filesystem;

class TomlConfigHandler {
    public:
        std::expected<CompilerConfig, Diagnostic> parseOrFail();

        bool createTomlFile( std::string_view projectName );
    private:
        std::expected<fs::path, Diagnostic> findProjectRoot( fs::path start );
};
