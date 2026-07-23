/* === Main Import === */

#include "Semantics/DeclarationPass.hpp"

/* === Declarations === */

#include <variant>
#include <string>

/* === Imports === */

#include "Driver/CompilationUnit.hpp"
#include "Core/CompilerContext.hpp"
#include "AST/BlockStatement.hpp"
#include "AST/Block.hpp"
#include "AST/Return.hpp"
#include "AST/Identifier.hpp"
#include "AST/VariableDeclaration.hpp"
#include "AST/FunctionDeclaration.hpp"

/* === Declaration Pass Methods === */

void DeclarationPass::run( CompilationUnit& compUnit ) {
    m_compUnit = &compUnit;

    auto statements = compUnit.ast().getStatements();

    m_compUnit->context().enterScope(InvalidNodeId, ScopeOwnerKind::Module);

    for ( auto& statement : statements ) {
        statement->accept(*this);
    }
}

void DeclarationPass::visit( const Literal& )
{
    // No need to do anything on this pass
}

void DeclarationPass::visit( const Assignment& )
{

}

void DeclarationPass::visit( const Identifier& )
{
    // No need to do anything on this pass
}

void DeclarationPass::visit( const ExpressionStatement& )
{

}

void DeclarationPass::visit( const BinaryExpression& )
{

}

void DeclarationPass::visit( const Block& block )
{
    for (Statement* stmt : block.statements)
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
    if (returnStmt.value) returnStmt.value->accept(*this);
}

void DeclarationPass::visit( const VariableDeclaration& varDec )
{
}

void DeclarationPass::visit( const FunctionDeclaration& funDec )
{
    Type* type = m_compUnit->context().allocate<Type>(
        TypeKind::Function,
        TypeState::Unresolved,
        TypeOrigin::Derived,
        std::monostate{}
    );

    TypeId typeId = m_compUnit->context().types().add( type );

    m_compUnit->context().nodeSemantics().bindType( funDec.id, typeId );

    FunctionSymbol* funSymbol = m_compUnit->context().allocate<FunctionSymbol>( std::string( funDec.identifier->name ), typeId ); 
    
    SymbolId symbolId = m_compUnit->context().symbols().add( funSymbol );

    m_compUnit->context().nodeSemantics().bindSymbol( funDec.id, symbolId );
}

void DeclarationPass::visit( const FunctionLiteral& )
{

}

void DeclarationPass::visit( const IfConditional& )
{

}

void DeclarationPass::visit( const ForLoop& )
{

}

void DeclarationPass::visit( const Range& )
{

}

void DeclarationPass::visit( const Unary& )
{

}

void DeclarationPass::visit( const FunctionCall& )
{

}

void DeclarationPass::visit( const ParsedType& )
{

}

void DeclarationPass::visit( const Parameter& )
{

}

void DeclarationPass::visit( const ModuleDeclaration& )
{

}

void DeclarationPass::visit( const ImportedSymbol& )
{

}

void DeclarationPass::visit( const Import& )
{

}

void DeclarationPass::visit( const QualifiedName& )
{

}