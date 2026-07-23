#pragma once

/* === Declarations === */

#include <vector>

/* === Imports === */

#include "Core/Ids.hpp"
#include "Scopes/ScopeStack.hpp"
#include "AST/ASTNode.hpp"

/* === Forward Declarations === */

class CompilationUnit;
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
struct ImportedSymbol;
struct Import;
struct QualifiedName;

/* === Declaration Pass === */

class DeclarationPass : public ASTVisitor
{
    public:
        void run( CompilationUnit& compUnit );

    private:
        CompilationUnit* m_compUnit = nullptr;

        void visit(const Literal&) override;
        void visit(const Assignment&) override;
        void visit(const Identifier&) override;
        void visit(const ExpressionStatement&) override;
        void visit(const BinaryExpression&) override;
        void visit(const Block&) override;
        void visit(const Break&) override;
        void visit(const Continue&) override;
        void visit(const Return&) override;
        void visit(const VariableDeclaration&) override;
        void visit(const FunctionDeclaration&) override;
        void visit(const FunctionLiteral&) override;
        void visit(const IfConditional&) override;
        void visit(const ForLoop&) override;
        void visit(const Range&) override;
        void visit(const Unary&) override;
        void visit(const FunctionCall&) override;
        void visit(const ParsedType&) override;
        void visit(const Parameter&) override;
        void visit(const ModuleDeclaration&) override;
        void visit(const ImportedSymbol&) override;
        void visit(const Import&) override;
        void visit(const QualifiedName&) override;
};