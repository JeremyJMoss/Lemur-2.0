#pragma once

/* === Dependencies === */

#include <string>


/* === Imports === */

#include "Tokens/TokenKeyword.hpp"
#include "Tokens/TokenKind.hpp"
#include "Tokens/TokenSymbol.hpp"

/* === Token Utility === */

inline std::string_view toString( const TokenKind kind ) 
{
    switch ( kind ) 
    {
        case TokenKind::Identifier:       return "Identifier";
        case TokenKind::Keyword:          return "Keyword";
        case TokenKind::Symbol:           return "Symbol";
        case TokenKind::Integer:          return "Integer";
        case TokenKind::Float:            return "Float";
        case TokenKind::Char:             return "Char";
        case TokenKind::Boolean:          return "Boolean";
        case TokenKind::String:           return "String";
        case TokenKind::MultiLineComment: return "MultiLineComment";
        case TokenKind::EndOfFile:        return "EndOfFile";
        case TokenKind::Unknown:          return "Unknown";
        case TokenKind::None:             return "None";
        default:                          return "Invalid TokenKind";
    }
}

inline std::string_view toString( const TokenSymbol symbol ) 
{
    switch ( symbol ) 
    {
        case TokenSymbol::None:           return "";
        case TokenSymbol::LParens:        return "(";
        case TokenSymbol::RParens:        return ")";
        case TokenSymbol::LBracket:       return "[";
        case TokenSymbol::RBracket:       return "]";
        case TokenSymbol::LBrace:         return "{";
        case TokenSymbol::RBrace:         return "}";
        case TokenSymbol::Dot:            return ".";
        case TokenSymbol::Comma:          return ",";
        case TokenSymbol::Equals:         return "==";
        case TokenSymbol::Greater:        return ">";
        case TokenSymbol::GreaterEquals:  return ">=";
        case TokenSymbol::LessEquals:     return "<=";
        case TokenSymbol::Less:           return "<";
        case TokenSymbol::Not:            return "!";
        case TokenSymbol::NotEquals:      return "!=";
        case TokenSymbol::Assign:         return "=";
        case TokenSymbol::Percent:        return "%";
        case TokenSymbol::Plus:           return "+";
        case TokenSymbol::Minus:          return "-";
        case TokenSymbol::Star:           return "*";
        case TokenSymbol::Slash:          return "/";
        case TokenSymbol::Or:             return "||";
        case TokenSymbol::And:            return "&&";
        case TokenSymbol::SemiColon:      return ";";
        case TokenSymbol::Colon:          return ":";
        case TokenSymbol::Arrow:          return "->";
        default:                          return "Invalid TokenSymbol";
    }
}

inline std::string_view toString( const TokenKeyword keyword ) 
{
    switch ( keyword ) 
    {
        case TokenKeyword::None:       return "None";
        case TokenKeyword::Infer:      return "infer";
        case TokenKeyword::Int:        return "int";
        case TokenKeyword::Float:      return "float";
        case TokenKeyword::Bool:       return "bool";
        case TokenKeyword::String:     return "string";
        case TokenKeyword::Null:       return "null";
        case TokenKeyword::If:         return "if";
        case TokenKeyword::Else:       return "else";
        case TokenKeyword::Return:     return "return";
        case TokenKeyword::Lock:       return "lock";
        case TokenKeyword::Fn:         return "fn";
        case TokenKeyword::For:        return "for";
        case TokenKeyword::Until:      return "until";
        case TokenKeyword::To:         return "to";
        case TokenKeyword::Step:       return "step";
        case TokenKeyword::Where:      return "where";
        case TokenKeyword::Break:      return "break";
        case TokenKeyword::Continue:   return "continue";
        case TokenKeyword::Wref:       return "wref";
        case TokenKeyword::Owned:      return "owned";
        case TokenKeyword::Shared:     return "shared";
        case TokenKeyword::Rref:       return "rref";
        case TokenKeyword::Weak:       return "weak";
        case TokenKeyword::In:         return "in";
        case TokenKeyword::Entry:      return "entry";
        case TokenKeyword::Module:     return "module";
        case TokenKeyword::Import:     return "import";
        case TokenKeyword::From:       return "from";
        case TokenKeyword::As:         return "as";
        case TokenKeyword::Export:     return "export";
        default:                       return "Invalid Keyword";
    }
}