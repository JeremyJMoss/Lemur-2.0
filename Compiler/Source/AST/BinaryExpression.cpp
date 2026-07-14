#include <string>
#include <unordered_map>
#include <format>
#include "Errors/Errors.hpp"
#include "AST/BinaryExpression.hpp"

BinaryOperator BinaryExpression::parseOperator( std::string_view op ) 
{
    const std::unordered_map<std::string_view, BinaryOperator> op_map = {
        {"+",  BinaryOperator::Plus},
        {"-",  BinaryOperator::Minus},
        {"*",  BinaryOperator::Multiply},
        {"/",  BinaryOperator::Divide},
        {"<",  BinaryOperator::LessThan},
        {"<=", BinaryOperator::LessThanOrEqualTo},
        {">",  BinaryOperator::GreaterThan},
        {">=", BinaryOperator::GreaterThanOrEqualTo},
        {"==", BinaryOperator::Equal},
        {"!=", BinaryOperator::NotEqual},
        {"&&", BinaryOperator::And},
        {"||", BinaryOperator::Or},
        {"%", BinaryOperator::Remainder}
    };

    auto it = op_map.find( op );

    if ( it != op_map.end() ) 
    {
        return it->second;
    }

    throw InternalCompilerError( 
        std::format( "Invalid binary operator: {}.\nPlease report this bug.", op )
    );
}