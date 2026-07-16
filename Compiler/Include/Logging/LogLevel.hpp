#pragma once

/* === Dependencies === */

#include <cstdint>
#include <string>

/* === Log Level === */

enum class LogLevel : uint8_t
{
    ERROR = 0,
    WARN,
    INFO,
    DEBUG,
    TRACE
};

std::string_view toString( LogLevel level );