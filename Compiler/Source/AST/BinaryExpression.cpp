#include <string>
#include <unordered_map>
#include <stdexcept>
#include "AST/BinaryExpression.hpp"

BinaryOperators BinaryExpression::parseOperator( const std::string& op ) 
{
    const std::unordered_map<std::string, BinaryOperators> op_map = {
        {"+",  BinaryOperators::Plus},
        {"-",  BinaryOperators::Minus},
        {"*",  BinaryOperators::Multiply},
        {"/",  BinaryOperators::Divide},
        {"<",  BinaryOperators::LessThan},
        {"<=", BinaryOperators::LessThanOrEqualTo},
        {">",  BinaryOperators::GreaterThan},
        {">=", BinaryOperators::GreaterThanOrEqualTo},
        {"==", BinaryOperators::Equal},
        {"!=", BinaryOperators::NotEqual},
        {"&&", BinaryOperators::And},
        {"||", BinaryOperators::Or},
        {"%", BinaryOperators::Remainder}
    };

    auto it = op_map.find( op );

    if ( it != op_map.end() ) 
    {
        return it->second;
    }

    throw std::invalid_argument( "Invalid binary operator: " + op );
}