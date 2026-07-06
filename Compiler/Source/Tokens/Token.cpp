#include "Tokens/Token.hpp"

#include <algorithm>

bool Token::checkMatches( TokenKind inputType, TokenSymbol inputValue ) const 
{ 
    return inputType == m_type && inputValue == m_symbol; 
}

bool Token::checkMatches( TokenKind inputType, TokenKeyword inputValue ) const 
{ 
    return inputType == m_type && inputValue == m_keyword; 
}

bool Token::checkMatches( TokenKind inputType, std::vector<TokenSymbol>&& group ) const 
{ 
    if ( inputType != m_type ) return false;
    
    bool hasValue = std::any_of(
        group.begin(),
        group.end(),
        [&] ( const TokenSymbol& value ) 
        {
            return value == m_symbol;
        }
    );

    return hasValue;
}

bool Token::checkMatches( TokenKind inputType, std::vector<TokenKeyword>&& group ) const 
{ 
    if ( inputType != m_type ) return false;
    
    bool hasValue = std::any_of(
        group.begin(),
        group.end(),
        [&] ( const TokenKeyword& value ) 
        {
            return value == m_keyword;
        }
    );

    return hasValue;
}

bool Token::checkTypeMatches( TokenKind inputType ) const 
{ 
    return inputType == m_type; 
}

bool Token::checkValueMatches( TokenKeyword inputValue ) const 
{ 
    return inputValue == m_keyword; 
}

bool Token::checkValueMatches( TokenSymbol inputValue ) const 
{ 
    return inputValue == m_symbol; 
}

bool Token::checkValueMatches( std::string_view inputValue ) const 
{ 
    return inputValue == m_value; 
}

TokenKind Token::getType() const 
{ 
    return m_type; 
}

TokenKeyword Token::getKeyword() const 
{ 
    return m_keyword; 
}

TokenSymbol Token::getSymbol() const 
{ 
    return m_symbol; 
}

void Token::setType( TokenKind type ) 
{ 
    m_type = type; 

}
void Token::clearType() 
{ 
    m_type = TokenKind::None; 
}

const std::string& Token::getValue() const 
{ 
    return m_value; 
}

void Token::addToValue( std::string_view value ) 
{ 
    m_value += value; 
}

void Token::clearValue() 
{ 
    m_keyword = TokenKeyword::None;
    m_symbol = TokenSymbol::None;
    m_value.clear();
}

const SourceRange Token::getLocation() const 
{ 
    return m_location; 
}

void Token::setLocationStart( SourceLocation start ) 
{ 
    m_location.start = start; 
}

void Token::setLocationEnd( SourceLocation end ) 
{ 
    m_location.end = end; 
}

void Token::setLocation( SourceRange range ) 
{ 
    m_location = range; 
}

void Token::clear() 
{ 
    clearType();
    clearValue();
 }