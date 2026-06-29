#pragma once

/* === Imports === */

#include "AST/ASTNode.hpp"

/* === Continue === */

struct Continue : Statement 
{
    Continue() = default;

    void accept(ASTVisitor& v) const override { 
        return v.visit(*this);
    }
};