#pragma once

/* === Imports === */

#include <memory>
#include "AST/ASTNode.hpp"
#include "AST/Block.hpp"
#include "AST/VariableDeclaration.hpp"

/* === For Loop === */

struct ForLoop : Statement 
{
    std::unique_ptr<VariableDeclaration> loopVar;
    std::unique_ptr<Expression> iterable;
    std::unique_ptr<Expression> step;
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Block> body = nullptr;

    ForLoop( 
        std::unique_ptr<VariableDeclaration>&& loopVar, 
        std::unique_ptr<Expression>&& iterator, 
        std::unique_ptr<Expression>&& step, 
        std::unique_ptr<Expression>&& where, 
        std::unique_ptr<Block>&& body 
    ) : loopVar( std::move( loopVar ) ), iterable( std::move( iterator ) ), 
        step( std::move( step ) ), condition( std::move( where ) ), 
        body( std::move( body ) ) {}

    ASTNodeType type() const override { return ASTNodeType::ForLoop; }
};