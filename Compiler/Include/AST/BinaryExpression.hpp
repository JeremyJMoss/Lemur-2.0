#pragma once

/* === Imports === */

#include <string>
#include "AST/ASTNode.hpp"

/* === Enums === */

enum class BinaryOperators 
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
    const BinaryOperators op;
    const Expression* right;

    explicit BinaryExpression( 
        const Expression* left, 
        const std::string& oper,
        const Expression* right 
    ) : left( left ), 
    op( parseOperator(oper) ), 
    right( right ) {};

    ASTNodeType type() const override { return ASTNodeType::BinaryExpression; }
    
    static BinaryOperators parseOperator( const std::string& op );
};