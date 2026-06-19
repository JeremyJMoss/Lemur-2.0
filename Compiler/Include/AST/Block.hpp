#pragma once

/* === Imports === */

#include <vector>
#include <memory>
#include "AST/ASTNode.hpp"

/* === Forward Declarations === */

/* === Block === */

struct Block : Statement 
{
    std::vector<std::unique_ptr<Statement>> statements;

    explicit Block( std::vector<std::unique_ptr<Statement>>&& statements ) 
        : statements( std::move( statements ) ) {}

    ASTNodeType type() const override { return ASTNodeType::Block; }
};
