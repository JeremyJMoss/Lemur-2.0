#pragma once

/* === Imports === */

#include <memory>
#include <variant>
#include <regex>
#include "AST/ASTNode.hpp"
#include "Types/TypeInfo.hpp"

/* === Constants === */

using LiteralValue = std::variant<std::string, char, int, float, bool, std::monostate>;

/* === Literal === */

struct Literal : Expression 
{
    LiteralValue value;

    explicit Literal( LiteralValue&& val ) : value( std::move( val ) ) {}
    
    ASTNodeType type() const override { return ASTNodeType::Literal; }
};

/* === Utility === */

inline const std::string getCharToString( char c )
{
    switch (c) {
        case '\n': return "'\\n'";
        case '\t': return "'\\t'";
        case '\r': return "'\\r'";
        case '\0': return "'\\0'";
        case '\\': return "'\\\\'";
        case '\'': return "'\\\''";
        case '\"': return "'\\\"'";
        default:
            return "'" + std::string(1, c) + "'";
    }
}

inline std::string trimTrailingZeros( const std::string& str ) 
{
    return std::regex_replace( str, std::regex( R"((\.\d*?[1-9])0+|\.(?=0+$)|\.0+$)" ), "$1" );
}

inline const std::string toString( const LiteralValue& value ) 
{
    return std::visit( [] ( const auto& val ) -> std::string
    {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr ( std::is_same_v<T, bool> ) return val ? "true" : "false";

        else if constexpr ( std::is_same_v<T, int> ) return std::to_string( val );

        else if constexpr ( std::is_same_v<T, float> ) return trimTrailingZeros( std::to_string( val ) );

        else if constexpr ( std::is_same_v<T, std::string> ) return "\"" + val + "\"";

        else if constexpr ( std::is_same_v<T, char> ) return getCharToString( val );

        else if constexpr ( std::is_same_v<T, std::monostate> ) return "null";
        
        throw std::runtime_error( "Weird string literal" );

    }, value );
}