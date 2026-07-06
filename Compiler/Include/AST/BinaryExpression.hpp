#pragma once

/* === Imports === */

#include <string>
#include "AST/ASTNode.hpp"

/* === Enums === */

enum class BinaryOperator : std::uint8_t
{
    Plus,                 // "+"
    Minus,                // "-"
    Multiply,             // "*"
    Divide,               // "/"
    LessThan,             // "<"
    GreaterThan,          // ">"
    LessThanOrEqualTo,    // "<="
    GreaterThanOrEqualTo, // ">="
    Equal,                // "=="
    NotEqual,             // "!="
    And,                  // "&&"
    Or,                   // "||"
    Remainder             // "%"
};

/* === BinaryExpression === */

struct BinaryExpression : Expression 
{
    const Expression* left;
    const BinaryOperator op;
    const Expression* right;

    explicit BinaryExpression( 
        const Expression* left, 
        std::string_view oper,
        const Expression* right 
    ) : left( left ), 
        op( parseOperator( oper ) ), 
        right( right ) {}

    void accept( ASTVisitor& v ) const override 
    { 
        return v.visit( *this );
    }
    
    static BinaryOperator parseOperator( std::string_view op );
};