/* === Main Import === */

#include "Semantics/DeclarationPass.hpp"

/* === Declarations === */

#include <variant>
#include <string>

/* === Imports === */

#include "Driver/CompilationUnit.hpp"
#include "Core/CompilerContext.hpp"
#include "Errors/Errors.hpp"
#include "AST/BlockStatement.hpp"
#include "AST/BinaryExpression.hpp"
#include "AST/ExpressionStatement.hpp"
#include "AST/Block.hpp"
#include "AST/Return.hpp"
#include "AST/Identifier.hpp"
#include "AST/VariableDeclaration.hpp"
#include "AST/FunctionDeclaration.hpp"
#include "AST/FunctionLiteral.hpp"
#include "AST/Parameter.hpp"
#include "AST/Assignment.hpp"
#include "AST/IfConditional.hpp"
#include "AST/ForLoop.hpp"

/* === Declaration Pass Methods === */

void DeclarationPass::run( CompilationUnit& compUnit ) {
    m_compUnit = &compUnit;

    auto statements = compUnit.ast().getStatements();

    m_compUnit->context().enterScope( NodeId{}, ScopeOwnerKind::Module );

    for ( auto& statement : statements ) {
        statement->accept( *this );
    }
}

void DeclarationPass::visit( const Literal& lit )
{
    // No need to do anything on this pass
}

void DeclarationPass::visit( const Assignment& assign )
{
    assign.value->accept( *this );
}

void DeclarationPass::visit( const Identifier& id )
{
    // No need to do anything on this pass
}

void DeclarationPass::visit( const ExpressionStatement& exprStmt )
{
    exprStmt.expression->accept( *this );
}

void DeclarationPass::visit( const BinaryExpression& binExpr )
{
    binExpr.right->accept( *this );
    binExpr.left->accept( *this );
}

void DeclarationPass::visit( const Block& block )
{
    for ( Statement* stmt : block.statements )
    {
        stmt->accept(*this);
    }
}

void DeclarationPass::visit( const BlockStatement& blockStmt )
{
    m_compUnit->context().enterScope( blockStmt.id, ScopeOwnerKind::Block );

    blockStmt.block->accept(*this);

    m_compUnit->context().leaveScope();
}

void DeclarationPass::visit( const Break& breakStmt )
{
    // No need to do anything this pass
}

void DeclarationPass::visit( const Continue& contineuStmt )
{
    // No need to do anything this pass
}

void DeclarationPass::visit( const Return& returnStmt )
{
    if ( returnStmt.value ) returnStmt.value->accept(*this);
}

void DeclarationPass::visit( const VariableDeclaration& varDec )
{
    CompilerContext& ctx = m_compUnit->context();

    if ( varDec.identifier->name.empty() ) throw InternalCompilerError( "Missing variable name during semantic analysis.\nPlease report this bug." );

    // Create Variable Symbol
    VariableSymbol* varSymbol = ctx.allocate<VariableSymbol>( std::string( varDec.identifier->name ), TypeId{} );

    // Add symbol to symbol table
    SymbolId symbolId = ctx.symbols().add( varSymbol );

    // Add relationship between node and symbol
    ctx.nodeSemantics().bindSymbol( varDec.id, symbolId );

     // Declare symbol in current scope
    auto declared = ctx.declareInScope( ctx.currentScope(), varDec.identifier->name, symbolId );

    if ( !declared ) ctx.errors().report( declared.error() );
}

void DeclarationPass::visit( const FunctionDeclaration& funDec )
{
    CompilerContext& ctx = m_compUnit->context();

    if ( funDec.identifier->name.empty() ) throw InternalCompilerError( "Missing function name during semantic analysis.\nPlease report this bug." );

    // Create Function Symbol
    FunctionSymbol* funSymbol = ctx.allocate<FunctionSymbol>( std::string( funDec.identifier->name ), TypeId{} ); 
    
    // Add symbol to symbol table
    SymbolId symbolId = ctx.symbols().add( funSymbol );

    // Add relationship between node and symbol
    ctx.nodeSemantics().bindSymbol( funDec.id, symbolId );

    // Declare symbol in current scope
    auto declared = ctx.declareInScope( ctx.currentScope(), funDec.identifier->name, symbolId );

    if ( !declared )
    {
        ctx.errors().report( declared.error() );
        return;
    }

    if ( !funDec.hasImplementation ) throw InternalCompilerError( "Missing implementation for function declaration.\nPlease report this bug." );

    // Enter scope for Function
    ctx.enterScope( funDec.body->id, ScopeOwnerKind::Function );

    for( auto& parameter : funDec.parameters )
    {
        parameter->accept( *this );
    }

    // Go over body statements
    funDec.body->accept( *this );

    // Leave function scope
    ctx.leaveScope();
}

void DeclarationPass::visit( const FunctionLiteral& funLit )
{
    CompilerContext& ctx = m_compUnit->context();

    ctx.enterScope( funLit.body->id, ScopeOwnerKind::Function );

    for ( auto& parameter : funLit.parameters )
    {
        parameter->accept( *this );
    }

    funLit.body->accept( *this );

    ctx.leaveScope();
}

void DeclarationPass::visit( const IfConditional& ifCond )
{
    CompilerContext& ctx = m_compUnit->context();

    // Collect declarations within condition
    ctx.enterScope( ifCond.id, ScopeOwnerKind::If );

    ifCond.condition->accept( *this );

    ctx.enterScope( ifCond.then->id, ScopeOwnerKind::Block );

    ifCond.then->accept( *this );

    ctx.leaveScope();

    if ( ifCond.elseStatement )
    {
        ifCond.elseStatement->accept( *this );
    }

    ctx.leaveScope();
}

void DeclarationPass::visit( const ForLoop& forL )
{
    CompilerContext& ctx = m_compUnit->context();

    ctx.enterScope( forL.id, ScopeOwnerKind::For );

    forL.loopVar->accept( *this );

    ctx.enterScope( forL.body->id, ScopeOwnerKind::Block );

    forL.body->accept( *this );

    ctx.leaveScope();

    ctx.leaveScope();
}

void DeclarationPass::visit( const Range& range )
{
    // No need to do anything this pass
}

void DeclarationPass::visit( const Unary& unary )
{
    // No need to do anything this pass
}

void DeclarationPass::visit( const FunctionCall& funCall )
{
    
}

void DeclarationPass::visit( const ParsedType& parsedType )
{
    // No need to do anything this pass
}

void DeclarationPass::visit( const Parameter& )
{

}

void DeclarationPass::visit( const ModuleDeclaration& )
{
    // No need to do anything this pass
}

void DeclarationPass::visit( const ImportedSymbol& )
{
    // No need to do anything this pass
}

void DeclarationPass::visit( const Import& )
{
    // No need to do anything this pass
}

void DeclarationPass::visit( const QualifiedName& qualName )
{
    // No need to do anything this pass
}