#include <string>
#include <memory>
#include <unordered_map>
#include <exception>
#include "AST/BinaryOperation.hpp"

BinaryOperation::BinaryOperation( 
    std::unique_ptr<Expression>&& left, 
    const std::string& oper, 
    std::unique_ptr<Expression>&& right 
) 
{
    this->left = std::move( left );
    op = BinaryOperation::parseOperator( oper );
    this->right = std::move( right );
}

BinaryOperators BinaryOperation::parseOperator( const std::string& op ) 
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