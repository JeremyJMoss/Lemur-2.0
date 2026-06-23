#pragma once

/* === Imports === */

#include <string>
#include <memory>
#include <vector>
#include "SourceControl/SourceLocation.hpp"

/* === Enums === */

// All AST Node Types
enum class ASTNodeType 
{
    ExpressionStatement,
    Identifier,
    Literal,
    Block,
    Return,
    Unary,
    Declaration,
    Assignment,
    FunctionLiteral,
    BinaryExpression,
    IfConditional,
    ForLoop,
    Range,
    Break,
    Continue,
    ParsedType,
    FunctionCall,
    FunctionDeclaration,
    VariableDeclaration,
    Parameter
};

/* === Base AST Node === */

using NodeId = std::size_t;

struct ASTNode 
{
    static inline NodeId nextId = 0;

    NodeId id;
    SourceRange location;
    virtual ~ASTNode() = default;
    virtual ASTNodeType type() const = 0;
    ASTNode(): id(nextId++) {}
};

/* === Derived AST Nodes === */

struct Expression : ASTNode {};

struct Statement : ASTNode {};

struct Declaration : Statement {};

/* === AST === */

struct AST {
    std::vector<std::unique_ptr<Statement>> m_statements;

    void addStatement( std::unique_ptr<Statement>&& statement ) { m_statements.push_back(std::move(statement)); }
};

/* === Utility === */

inline const std::string toString( const ASTNodeType& type ) 
{
    switch ( type ) 
    {
        case ASTNodeType::ExpressionStatement:  return "Expression Statement";
        case ASTNodeType::Identifier:           return "Identifier";
        case ASTNodeType::Literal:              return "Literal";
        case ASTNodeType::Block:                return "Block";
        case ASTNodeType::Return:               return "Return";
        case ASTNodeType::Unary:                return "Unary";
        case ASTNodeType::Declaration:          return "Declaration";
        case ASTNodeType::Assignment:           return "Assignment";
        case ASTNodeType::FunctionLiteral:      return "Function Literal";
        case ASTNodeType::BinaryExpression:      return "Binary Expression";
        case ASTNodeType::IfConditional:        return "If Conditional";
        case ASTNodeType::ForLoop:              return "For Loop";
        case ASTNodeType::Range:                return "Range";
        case ASTNodeType::Break:                return "Break";
        case ASTNodeType::Continue:             return "Continue";
        case ASTNodeType::ParsedType:           return "Parsed Type";
        case ASTNodeType::FunctionCall:         return "Function Call";
        case ASTNodeType::FunctionDeclaration:  return "Function Declaration";
        case ASTNodeType::VariableDeclaration:  return "Variable Declaration";
        default:                                return "Unknown";
    }
};