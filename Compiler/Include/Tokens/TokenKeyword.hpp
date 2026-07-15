#pragma once

/* === Dependencies === */

#include <cstdint>

/* === Token Keyword === */

enum class TokenKeyword : uint8_t
{
    None,
    Infer,
    Int,
    Float,
    Bool,
    String,
    Null,
    If,
    Else,
    Return,
    Lock,
    Fn,
    For,
    Until,
    To,
    Step,
    Where,
    Break, 
    Continue,
    Wref,
    Owned,
    Shared,
    Rref,
    Weak,
    In,
    Entry,
    Module,
    Import,
    From,
    As,
    Export
};