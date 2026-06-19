#pragma once

/* === Imports === */

#include "AST/ASTNode.hpp"

/* === Break === */

struct Break : Statement 
{
    Break() = default;

    ASTNodeType type() const override { return ASTNodeType::Break; }
};
