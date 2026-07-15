#pragma once

/* === Dependencies === */

#include <cstdint>

/* === Token Kind === */

enum class TokenKind : uint8_t
{
    Identifier,
    Keyword,
    Symbol,
    Integer,
    Float,
    Char,
    Boolean,
    String,
    MultiLineComment,
    EndOfFile,
    Unknown,
    None
};