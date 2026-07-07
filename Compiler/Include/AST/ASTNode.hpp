#pragma once

/* === Imports === */

#include <string>
#include <vector>
#include "SourceControl/SourceLocation.hpp"

/* === Forward Declarations === */

struct Literal;
struct Assignment;
struct Identifier;
struct BinaryExpression;
struct ExpressionStatement;
struct Block;
struct Break;
struct Continue;
struct Return;
struct VariableDeclaration;
struct FunctionDeclaration;
struct FunctionLiteral;
struct IfConditional;
struct ForLoop;
struct Range;
struct Unary;
struct FunctionCall;
struct ParsedType;
struct Parameter;
struct ModuleDeclaration;

/* === Visitor === */

// Visitor struct for logging and debugging
struct ASTVisitor
{
    virtual void visit(const Literal&) = 0;
    virtual void visit(const Assignment&) = 0;
    virtual void visit(const Identifier&) = 0;
    virtual void visit(const ExpressionStatement&) = 0;
    virtual void visit(const BinaryExpression&) = 0;
    virtual void visit(const Block&) = 0;
    virtual void visit(const Break&) = 0;
    virtual void visit(const Continue&) = 0;
    virtual void visit(const Return&) = 0;
    virtual void visit(const VariableDeclaration&) = 0;
    virtual void visit(const FunctionDeclaration&) = 0;
    virtual void visit(const FunctionLiteral&) = 0;
    virtual void visit(const IfConditional&) = 0;
    virtual void visit(const ForLoop&) = 0;
    virtual void visit(const Range&) = 0;
    virtual void visit(const Unary&) = 0;
    virtual void visit(const FunctionCall&) = 0;
    virtual void visit(const ParsedType&) = 0;
    virtual void visit(const Parameter&) = 0;
    virtual void visit(const ModuleDeclaration&) = 0;
    virtual ~ASTVisitor() = default;
};

/* === Base AST Node === */
using NodeId = std::size_t;

struct ASTNode 
{
    static inline NodeId nextId = 0;

    const NodeId id;
    SourceRange location;
    virtual ~ASTNode() = default;
    virtual void accept( ASTVisitor& v ) const = 0;
    // allow auto increment of NodeId
    ASTNode(): id( nextId++ ) {}
};

/* === Derived AST Nodes === */

// Produces a value when evaluated.
struct Expression : ASTNode {};

// Performs an action during program execution.
struct Statement : ASTNode {};

// Declares a symbol (e.g. variable, function, type, or module) within a scope.
struct Declaration : Statement {};

/* === Abstract Syntax Tree === */

struct AST {
    std::vector<const Statement*> m_statements;

    void addStatement( Statement* statement ) 
    { 
        m_statements.push_back( statement ); 
    }
};