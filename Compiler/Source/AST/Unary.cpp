#include <string>
#include <format>
#include "Errors/Errors.hpp"
#include "AST/Unary.hpp"

UnaryOperator Unary::parseUnaryOperator( std::string_view op ) 
{
    if ( op == "!" )      
        return UnaryOperator::Not;
    else if ( op == "-" ) 
        return UnaryOperator::Negate;
    else 
        throw InternalCompilerError( 
            std::format( "Invalid unary operator: {}.\nPlease report this bug.", op ) 
        );
}