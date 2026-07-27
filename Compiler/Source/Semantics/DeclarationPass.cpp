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
#include "AST/Unary.hpp"
#include "AST/FunctionCall.hpp"
#include "AST/Range.hpp"
#include "AST/Import.hpp"
#include "AST/QualifiedName.hpp"
#include "Modules/ModuleInfo.hpp"
#include "AST/ImportedSymbol.hpp"

/* === Declaration Pass Methods === */

void DeclarationPass::run( CompilationUnit& compUnit ) {
    m_compUnit = &compUnit;

    auto statements = compUnit.ast().getStatements();

    m_compUnit->context().enterScope( NodeId{}, ScopeOwnerKind::Module );

    for ( auto& statement : statements ) 
    {
        if ( !statement ) throw InternalCompilerError( "Missing statement in ast tree statement list.\nPlease report this bug." );

        statement->accept( *this );
    }

    m_compUnit->context().leaveScope();
}

void DeclarationPass::visit( const Literal& )
{
    // No need to do anything on this pass
}

void DeclarationPass::visit( const Assignment& assign )
{
    if ( !assign.identifier ) throw InternalCompilerError( "Missing identifier for assignment.\nPlease report this bug." );
    assign.identifier->accept( *this );

    if ( !assign.value ) throw InternalCompilerError( "Missing value for assignment.\nPlease report this bug." );
    assign.value->accept( *this );
}

void DeclarationPass::visit( const Identifier& )
{
    // No need to do anything on this pass
}

void DeclarationPass::visit( const ExpressionStatement& exprStmt )
{
    if ( !exprStmt.expression ) throw InternalCompilerError( "Missing expression within expression statement.\nPlease report this bug" );
    exprStmt.expression->accept( *this );
}

void DeclarationPass::visit( const BinaryExpression& binExpr )
{
    if ( !binExpr.right ) throw InternalCompilerError( "Missing right side of binary expression.\nPlease report this bug." );
    binExpr.right->accept( *this );

    if ( !binExpr.left ) throw InternalCompilerError( "Missing left side of binary expression.\nPlease report this bug." );
    binExpr.left->accept( *this );
}

void DeclarationPass::visit( const Block& block )
{
    for ( Statement* stmt : block.statements )
    {
        if ( !stmt ) throw InternalCompilerError( "Missing statement in block list.\nPlease report this bug." );

        stmt->accept(*this);
    }
}

void DeclarationPass::visit( const BlockStatement& blockStmt )
{
    if ( !blockStmt.block ) throw InternalCompilerError( "Missing block within block statement.\nPlease report this bug." );

    m_compUnit->context().enterScope( blockStmt.id, ScopeOwnerKind::Block );

    blockStmt.block->accept(*this);

    m_compUnit->context().leaveScope();
}

void DeclarationPass::visit( const Break& )
{
    // No need to do anything this pass
}

void DeclarationPass::visit( const Continue& )
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

    if ( !varDec.identifier || varDec.identifier->name.empty() ) throw InternalCompilerError( "Missing variable name during semantic analysis.\nPlease report this bug." );

    // Create Variable Symbol
    VariableSymbol* varSymbol = ctx.allocate<VariableSymbol>( std::string( varDec.identifier->name ), TypeId{}, VariableStorage::Local, varDec.locked );

    // Add symbol to symbol table
    SymbolId symbolId = ctx.symbols().add( varSymbol );

    // Add relationship between node and symbol
    ctx.nodeSemantics().bindSymbol( varDec.id, symbolId );

     // Declare symbol in current scope
    auto declared = ctx.declareInScope( ctx.currentScope(), varDec.identifier->name, symbolId );

    if ( !declared ) ctx.errors().report( declared.error() );

    if ( varDec.visibility == DeclarationVisibility::Public )
    {
        bool success = m_compUnit->exports().add( varDec.identifier->name, symbolId );
        if ( !success ) 
        {
            ctx.errors().report( 
                Diagnostic(
                    "Unable to export symbol",
                    ErrorCategory::Linking,
                    ErrorSeverity::Error,
                    varDec.location
                )
            );
        }
    }


    if ( varDec.initialiser ) varDec.initialiser->accept( *this );
}

void DeclarationPass::visit( const FunctionDeclaration& funDec )
{
    CompilerContext& ctx = m_compUnit->context();

    if ( !funDec.identifier || funDec.identifier->name.empty() ) throw InternalCompilerError( "Missing function name during semantic analysis.\nPlease report this bug." );

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

    if ( !funDec.body ) throw InternalCompilerError( "Missing function body for function declaration.\nPlease report this bug." ); 

    // Enter scope for Function
    ctx.enterScope( funDec.body->id, ScopeOwnerKind::Function );

    for( auto& parameter : funDec.parameters )
    {
        if ( !parameter ) throw InternalCompilerError( "Missing parameter in function declaration parameter list.\nPlease report this bug." );

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

    if ( !funLit.body ) throw InternalCompilerError( "Missing function body for function literal.\nPlease report this bug." );

    ctx.enterScope( funLit.body->id, ScopeOwnerKind::Function );

    for ( auto& parameter : funLit.parameters )
    {
        if ( !parameter ) throw InternalCompilerError( "Missing parameter in function literal parameter list.\nPlease report this bug." );

        parameter->accept( *this );
    }

    funLit.body->accept( *this );

    ctx.leaveScope();
}

void DeclarationPass::visit( const IfConditional& ifCond )
{
    CompilerContext& ctx = m_compUnit->context();

    if ( !ifCond.then ) throw InternalCompilerError( "Missing then block for if conditional.\nPlease report this bug." );

    // Collect declarations within condition
    ctx.enterScope( ifCond.id, ScopeOwnerKind::If );

    if ( ifCond.condition ) ifCond.condition->accept( *this );

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

    if ( !forL.loopVar ) throw InternalCompilerError( "Missing loop variable for for loop.\nPlease report this bug." );

    if ( !forL.body ) throw InternalCompilerError( "Missing body for for loop.\nPlease report this bug." );

    ctx.enterScope( forL.id, ScopeOwnerKind::For );

    forL.loopVar->accept( *this );

    ctx.enterScope( forL.body->id, ScopeOwnerKind::Block );

    forL.body->accept( *this );

    ctx.leaveScope();

    ctx.leaveScope();
}

void DeclarationPass::visit( const Range& range )
{
    if ( !range.start ) throw InternalCompilerError( "Missing start expression for range.\nPlease report this bug.");
    range.start->accept( *this );

    if ( !range.end ) throw InternalCompilerError( "Missing end expression for range.\nPlease report this bug.");
    range.end->accept( *this );
}

void DeclarationPass::visit( const Unary& unary )
{
    if ( !unary.argument ) throw InternalCompilerError( "Missing argument for unary expression.\nPlease report this bug.");

    unary.argument->accept( *this );
}

void DeclarationPass::visit( const FunctionCall& funCall )
{
    if ( !funCall.callee ) throw InternalCompilerError( "Missing callee for function call.\nPlease report this bug." );

    funCall.callee->accept( *this );

    for ( auto arg : funCall.arguments )
    {
        if ( !arg ) throw InternalCompilerError( "Missing argument in function call.\nPlease report this bug." );

        arg->accept( *this );
    }
}

void DeclarationPass::visit( const ParsedType& )
{
    // No need to do anything this pass
}

void DeclarationPass::visit( const Parameter& param )
{
    CompilerContext& ctx = m_compUnit->context();

    if ( !param.identifier || param.identifier->name.empty() ) throw InternalCompilerError( "Missing parameter name during semantic analysis.\nPlease report this bug." );

    // Create Variable Symbol
    VariableSymbol* paramSymbol = ctx.allocate<VariableSymbol>( std::string( param.identifier->name ), TypeId{}, VariableStorage::Parameter );

    // Add symbol to symbol table
    SymbolId symbolId = ctx.symbols().add( paramSymbol );

    // Add relationship between node and symbol
    ctx.nodeSemantics().bindSymbol( param.id, symbolId );

     // Declare symbol in current scope
    auto declared = ctx.declareInScope( ctx.currentScope(), param.identifier->name, symbolId );

    if ( !declared ) ctx.errors().report( declared.error() );
}

void DeclarationPass::visit( const ModuleDeclaration& )
{
    // No need to do anything ever
}

void DeclarationPass::visit( const ImportedSymbol& importSym )
{
    CompilerContext& ctx = m_compUnit->context();

    if (!importSym.name || importSym.name->name.empty() ) throw InternalCompilerError( "Missing import symbol name during semantic analysis.\nPlease report this bug." );

    const SymbolId symbolId = m_currentImportingModule->exports.find( importSym.name->name );

    std::string symbolAlias = importSym.alias.has_value() ? importSym.alias.value()->name : importSym.name->name;

    ImportSymbol* importSymbol = ctx.allocate<ImportSymbol>(symbolAlias, symbolId );

    SymbolId symbolId = ctx.symbols().add( importSymbol );

    // Add relationship between node and symbol
    ctx.nodeSemantics().bindSymbol( importSym.id, symbolId );

    auto declared = ctx.declareInScope( ctx.currentScope(), symbolAlias, symbolId );

    if ( !declared ) ctx.errors().report( declared.error() );
}

void DeclarationPass::visit( const Import& importStmt )
{
    CompilerContext& ctx = m_compUnit->context();

    if (!importStmt.moduleName ||  importStmt.moduleName->name.empty() ) throw InternalCompilerError( "Missing module name for import statement during semantic analysis.\nPlease report this bug." );

    const ModuleInfo* moduleInfo = ctx.modules().find( std::string_view( importStmt.moduleName->name ) );

    if ( importStmt.importedSymbols.empty() )
    {
        std::string moduleAlias = importStmt.alias.has_value() ? importStmt.alias.value()->name : importStmt.moduleName->name;

        // Create Module Symbol
        ModuleSymbol* moduleSymbol = ctx.allocate<ModuleSymbol>( moduleAlias, moduleInfo->header.id );

        // Add symbol to symbol table
        SymbolId symbolId = ctx.symbols().add( moduleSymbol );
        
        // Add relationship between node and symbol
        ctx.nodeSemantics().bindSymbol( importStmt.id, symbolId );

        auto declared = ctx.declareInScope( ctx.currentScope(), moduleAlias, symbolId );

        if ( !declared ) ctx.errors().report( declared.error() );
    }
    else
    {
        m_currentImportingModule = moduleInfo;

        for ( auto& importSym : importStmt.importedSymbols )
        {
            importSym->accept( *this );
        }
    }
}

void DeclarationPass::visit( const QualifiedName& )
{
    // No need to do anything this pass
}