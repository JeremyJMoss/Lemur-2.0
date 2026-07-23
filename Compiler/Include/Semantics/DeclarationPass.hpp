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

        void visit( const Literal& lit ) override;
        void visit( const Assignment& assign ) override;
        void visit( const Identifier& id ) override;
        void visit( const ExpressionStatement& exprStmt ) override;
        void visit( const BinaryExpression& binExpr ) override;
        void visit( const Block& block ) override;
        void visit( const BlockStatement& BlockStmt ) override;
        void visit( const Break& breakStmt ) override;
        void visit( const Continue& continueStmt ) override;
        void visit( const Return& returnStmt ) override;
        void visit( const VariableDeclaration& varDec ) override;
        void visit( const FunctionDeclaration& funcDec ) override;
        void visit( const FunctionLiteral& funLit ) override;
        void visit( const IfConditional& ifCond ) override;
        void visit( const ForLoop& forL ) override;
        void visit( const Range& range ) override;
        void visit( const Unary& unary ) override;
        void visit( const FunctionCall& funCall ) override;
        void visit( const ParsedType& parType ) override;
        void visit( const Parameter& param ) override;
        void visit( const ModuleDeclaration& modDec ) override;
        void visit( const ImportedSymbol& impSym ) override;
        void visit( const Import& import ) override;
        void visit( const QualifiedName& qualName ) override;
};