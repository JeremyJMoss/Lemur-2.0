#pragma once

/* === Imports === */

#include <string>
#include <memory>
#include <vector>
#include "Types/TypeInfo.hpp"
#include "Utils/SourceLocation.hpp"

/* === Forward Declarations === */

struct Symbol;

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
    BinaryOperation,
    IfConditional,
    ForLoop,
    Range,
    Break,
    Continue,
    ParsedType,
    FunctionCall,
    FunctionDeclaration,
    VariableDeclaration
};

/* === Base AST Node === */

struct ASTNode 
{
    SourceRange location;
    virtual ~ASTNode() = default;
    virtual ASTNodeType type() const = 0;
};

/* === Derived AST Nodes === */

struct Expression : ASTNode {
    std::shared_ptr<TypeInfo> resolvedType;

    bool isLValue = false; // Is this assignable

    bool isAssignable() { return isLValue; }
};

struct Statement : ASTNode {};

struct Declaration : Statement {
    std::shared_ptr<Symbol> boundSymbol;
};

/* === AST === */

struct AST {
    std::vector<std::unique_ptr<Statement>> statements;

    AST( std::vector<std::unique_ptr<Statement>>&& statements ) 
        : statements( std::move( statements ) ) {}
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
        case ASTNodeType::BinaryOperation:      return "Binary Operation";
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