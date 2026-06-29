#pragma once

/* === Imports === */

#include <string>
#include <vector>
#include "SourceControl/SourceLocation.hpp"

/* === Base AST Node === */

using NodeId = std::size_t;

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
    virtual ~ASTVisitor() = default;
};

struct ASTNode 
{
    static inline NodeId nextId = 0;

    const NodeId id;
    SourceRange location;
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& v) const = 0;
    ASTNode(): id(nextId++) {}
};

/* === Derived AST Nodes === */

struct Expression : ASTNode {};

struct Statement : ASTNode {};

struct Declaration : Statement {};

/* === AST === */

struct AST {
    std::vector<const Statement*> m_statements;

    void addStatement( Statement* statement ) { m_statements.push_back( statement ); }
};