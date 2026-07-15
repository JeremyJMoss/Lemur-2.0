#pragma once

/* === Dependencies ===*/

#include <optional>
#include <vector>

/* === Imports === */

#include "AST/ASTNode.hpp"

/* === Forward Declarations === */

struct Identifier;
struct ImportedSymbol;
struct QualifiedName;

/* === Import === */

struct Import : Statement
{
    QualifiedName* moduleName;
    std::optional<Identifier*> alias;
    std::vector<ImportedSymbol*> importedSymbols;

    Import( QualifiedName* moduleName, std::optional<Identifier*> alias, std::vector<ImportedSymbol*> importedSymbols ) 
        : moduleName( moduleName ),
          alias( alias ),
          importedSymbols( std::move( importedSymbols ) ) {}

    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};