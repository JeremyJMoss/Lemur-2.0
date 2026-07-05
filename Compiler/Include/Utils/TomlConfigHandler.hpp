#pragma once

#include "Config/Config.hpp"
#include "Errors/Errors.hpp"
#include <expected>

class TomlConfigHandler {
    public:
        std::expected<CompilerConfig, ConfigError> parseOrFail();

        bool createTomlFile(const std::string& projectName );
};
