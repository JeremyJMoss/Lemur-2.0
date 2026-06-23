#pragma once

/* === Imports === */

#include <string>
#include <memory>
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
    BinaryOperators op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    explicit BinaryExpression( 
        std::unique_ptr<Expression>&& left, 
        const std::string& oper,
        std::unique_ptr<Expression>&& right 
    );

    ASTNodeType type() const override { return ASTNodeType::BinaryExpression; }

    private:
        static BinaryOperators parseOperator( const std::string& op );
};

/* === Utility === */

inline const std::string toString( const BinaryOperators& biop ) 
{
    switch ( biop ) 
    {
        case BinaryOperators::Plus:                 return "+";
        case BinaryOperators::Minus:                return "-";
        case BinaryOperators::Multiply:             return "*";
        case BinaryOperators::Divide:               return "/";
        case BinaryOperators::LessThan:             return "<";
        case BinaryOperators::LessThanOrEqualTo:    return "<=";
        case BinaryOperators::GreaterThan:          return ">";
        case BinaryOperators::GreaterThanOrEqualTo: return ">=";
        case BinaryOperators::Equal:                return "==";
        case BinaryOperators::NotEqual:             return "!=";
        case BinaryOperators::And:                  return "&&";
        case BinaryOperators::Or:                   return "||";
        case BinaryOperators::Remainder:            return "%";
        default:                                    return "Unknown";
    }
}