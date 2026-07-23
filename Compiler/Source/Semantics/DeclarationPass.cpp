/* === Main Import === */

#include "Semantics/DeclarationPass.hpp"
#include "Driver/CompilationUnit.hpp"

/* === Declaration Pass Methods === */

void DeclarationPass::run( CompilationUnit& compUnit ) {
    m_compUnit = &compUnit;

    auto statements = compUnit.ast().getStatements();

    for ( auto& statement : statements ) {
        statement->accept(*this);
    }
}