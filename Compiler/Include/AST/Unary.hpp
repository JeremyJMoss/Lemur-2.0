#pragma once

/* === Imports === */

#include <memory>
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
    std::unique_ptr<Expression> argument;
    UnaryOperator op;
    
    explicit Unary( std::unique_ptr<Expression>&& arg, const std::string& oper );

    ASTNodeType type() const override { return ASTNodeType::Unary; }

    private:
        UnaryOperator parseUnaryOperator( const std::string& op );
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