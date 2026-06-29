#include <string>
#include <stdexcept>
#include "AST/Unary.hpp"

UnaryOperator Unary::parseUnaryOperator( const std::string& op ) 
{
    if ( op == "!" ) return UnaryOperator::Not;
    else if ( op == "-" ) return UnaryOperator::Negate;
    else throw std::invalid_argument( std::string( "invalid unary operator: " ) + op );
}