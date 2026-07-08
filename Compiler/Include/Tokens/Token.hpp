#pragma once

/* === Imports === */

#include <string>
#include <vector>
#include "SourceControl/SourceLocation.hpp"

using TokenId = size_t;

/* === Enum Declarations === */

enum class TokenKind : u_int8_t
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

enum class TokenSymbol : u_int8_t
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

enum class TokenKeyword : u_int8_t
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
    From
};

/* === Utility === */

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
        default:                       return "Invalid Keyword";
    }
}

/* === Token === */

class Token 
{
    public:
        static inline TokenId nextId = 0;

        bool checkMatches( TokenKind inputType, TokenSymbol inputValue ) const;

        bool checkMatches( TokenKind inputType, TokenKeyword inputValue ) const;

        bool checkMatches( TokenKind inputType, std::vector<TokenSymbol>&& group ) const;

        bool checkMatches( TokenKind inputType, std::vector<TokenKeyword>&& group ) const;

        bool checkTypeMatches( TokenKind inputType ) const;

        bool checkValueMatches( TokenSymbol inputValue ) const;

        bool checkValueMatches( TokenKeyword inputValue ) const;

        bool checkValueMatches( std::string_view inputValue ) const;

        TokenId getId() { return m_id; }

        TokenKind getType() const;

        TokenKeyword getKeyword() const;

        TokenSymbol getSymbol() const;

        void setType( TokenKind type );

        void clearType();

        std::string_view getValue() const;

        void addToValue( std::string_view value );

        void clearValue();

        const SourceRange getLocation() const;

        void setLocationStart( SourceLocation start );

        void setLocationEnd( SourceLocation end );

        void setLocation( SourceRange range );

        void clear();

        Token() {}

        Token( TokenKind type, TokenSymbol symbol, std::string_view value, SourceRange location ) 
            : m_id( nextId++ ), m_type( type ), m_symbol( symbol ), m_value( value ), m_location( location ) {}

        Token( TokenKind type, TokenKeyword kw, std::string_view value, SourceRange location )
            : m_id( nextId++ ), m_type( type ), m_keyword( kw ), m_value( value ), m_location( location ) {}

        Token( TokenKind type, std::string_view value, SourceRange location )
            : m_id( nextId++ ), m_type( type ), m_value( value ), m_location( location ) {}
    
    private:
        TokenId m_id;
        TokenKind m_type = TokenKind::None;
        TokenSymbol m_symbol = TokenSymbol::None;
        TokenKeyword m_keyword = TokenKeyword::None;
        std::string m_value;
        SourceRange m_location;
};