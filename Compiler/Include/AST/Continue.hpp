#pragma once

/* === Imports === */

#include "AST/ASTNode.hpp"

/* === Continue === */

struct Continue : Statement 
{
    Continue() = default;

    ASTNodeType type() const override { return ASTNodeType::Continue; }
};