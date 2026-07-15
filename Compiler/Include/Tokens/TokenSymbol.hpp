#pragma once

/* === Dependencies === */

#include <cstdint>

/* === Token Symbol === */

enum class TokenSymbol : uint8_t
{
    None,
    LParens,       // )
    RParens,       // (
    LBracket,      // ]
    RBracket,      // [
    LBrace,        // }
    RBrace,        // {
    Dot,           // .
    Comma,         // ,
    Equals,        // ==
    Greater,       // >
    GreaterEquals, // >=
    LessEquals,    // <=
    Less,          // <
    Not,           // !
    NotEquals,     // !=
    Assign,        // =
    Percent,       // %
    Plus,          // +
    Minus,         // -
    Star,          // *
    Slash,         // /
    Or,            // ||
    And,           // &&
    SemiColon,     // ;
    Colon,         // :
    Arrow          // ->
};
