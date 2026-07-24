#pragma once

/* === Dependencies ===*/

#include <string>
#include <vector>

/* === Imports === */

#include "SourceControl/SourceLocation.hpp"
#include "Core/Ids.hpp"
#include "Tokens/TokenKind.hpp"
#include "Tokens/TokenSymbol.hpp"
#include "Tokens/TokenKeyword.hpp"

/* === Token === */

class Token 
{
    public:
        static inline size_t nextId = 0;

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
        TokenId m_id{};
        TokenKind m_type = TokenKind::None;
        TokenSymbol m_symbol = TokenSymbol::None;
        TokenKeyword m_keyword = TokenKeyword::None;
        std::string m_value;
        SourceRange m_location;
};