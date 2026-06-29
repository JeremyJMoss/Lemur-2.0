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

    void accept(ASTVisitor& v) const override { 
        return v.visit(*this);
    }
};
