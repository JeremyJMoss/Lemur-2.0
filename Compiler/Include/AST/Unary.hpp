#pragma once

/* === Dependencies ===*/

#include <string>

/* === Imports === */

#include "AST/ASTNode.hpp"

/* === Enum Declaration === */

enum class UnaryOperator : std::uint8_t
{
    Not,    // "!"
    Negate, // "-"
};

/* === Unary === */

struct Unary : Expression 
{
    const Expression* argument;
    const UnaryOperator op;
    
    explicit Unary( const Expression* arg, std::string_view oper )
        : argument( arg ), op( parseUnaryOperator( oper ) ) {}

    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }

    static UnaryOperator parseUnaryOperator( std::string_view op );
};