#pragma once

/* === Imports === */

#include <string>
#include "AST/ASTNode.hpp"

/* === Enums === */

enum class UnaryOperator 
{
    Not,    // "!"
    Negate, // "-"
};

/* === Unary === */

struct Unary : Expression 
{
    const Expression* argument;
    const UnaryOperator op;
    
    explicit Unary( const Expression* arg, const std::string& oper )
        : argument( arg ), op( parseUnaryOperator(oper) ) {}

    ASTNodeType type() const override { return ASTNodeType::Unary; }

    static UnaryOperator parseUnaryOperator( const std::string& op );
};

/* === Utiltity === */

inline const std::string toString( const UnaryOperator& unop ) 
{
    switch( unop ) 
    {
        case UnaryOperator::Negate: return "-";
        case UnaryOperator::Not:    return "!";
        default:                    return "Unknown";
    }
}