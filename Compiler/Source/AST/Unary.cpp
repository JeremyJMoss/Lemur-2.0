#include <string>
#include <memory>
#include "AST/Unary.hpp"

Unary::Unary( std::unique_ptr<Expression>&& arg, const std::string& oper ) 
{
    argument = std::move( arg );
    op = parseUnaryOperator( oper );
}

UnaryOperator Unary::parseUnaryOperator( const std::string& op ) 
{
    if ( op == "!" ) return UnaryOperator::Not;
    else if ( op == "-" ) return UnaryOperator::Negate;
    else throw std::invalid_argument( std::string( "invalid unary operator: " ) + op );
}