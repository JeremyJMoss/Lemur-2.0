#pragma once

/* === Imports === */

#include <vector>
#include "AST/ASTNode.hpp"

/* === Forward Declarations === */

/* === Block === */

struct Block : Statement 
{
    std::vector<const Statement*> statements;

    explicit Block( std::vector<const Statement*> statements ) 
        : statements( std::move( statements ) ) {}

    ASTNodeType type() const override { return ASTNodeType::Block; }
};
