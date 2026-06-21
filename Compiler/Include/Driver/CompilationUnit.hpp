#pragma once

#include <vector>
#include "AST/ASTNode.hpp"
#include "Symbols/Symbol.hpp"
#include "Symbols/SymbolTable.hpp"
#include "Types/TypeTable.hpp"
#include "Scopes/ScopeTable.hpp"

using NodeId = std::size_t;
using SymbolId = std::size_t;
using TypeId = std::size_t;
using ScopeId = std::size_t;

class CompilationUnit {
    
    ScopeId m_globalScope;
    ScopeId m_currentScope;

    AST m_ast;
    SymbolTable m_symbols;
    TypeTable m_types;
    ScopeTable m_scopes;

    std::unordered_map<NodeId, SymbolId> m_resolvedSymbols;
    std::unordered_map<NodeId, TypeId> m_resolvedTypes;
    std::unordered_map<NodeId, ScopeId> m_nodeScopes;
};