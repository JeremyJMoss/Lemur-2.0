#pragma once

#include <vector>
#include "AST/ASTNode.hpp"
#include "Symbols/Symbol.hpp"
#include "Symbols/SymbolTable.hpp"
#include "Types/TypeTable.hpp"
#include "Scopes/ScopeTable.hpp"
#include "Tokens/TokenTable.hpp"
#include "DataStructures/Arena.hpp"

using NodeId = std::size_t;
using SymbolId = std::size_t;
using TypeId = std::size_t;
using ScopeId = std::size_t;
using FileId = std::size_t;
using TokenId = std::size_t;

class CompilationUnit {
    public:
        CompilationUnit( FileId fileId ) : m_fileId( fileId ) {
            ScopeId scopeId = createScope(InvalidScopeId, ScopeOwnerKind::Global );
            m_globalScope = scopeId;
            m_currentScope = scopeId;
        }

        TokenId addToken( Token token ) {
            return m_tokens.add(std::move(token));
        }

        SymbolId addSymbol( Symbol symbol ) {
            return m_symbols.add(std::move(symbol));
        }

        void bindSymbol( NodeId nodeId, SymbolId symbolId ) {
            m_resolvedSymbols.emplace(nodeId, symbolId);
        }

        TypeId addType( Type type ) {
            return m_types.add(std::move(type));
        }

        void bindType( NodeId nodeId, TypeId typeId ) {
            m_resolvedTypes.emplace(nodeId, typeId);
        }

        ScopeId createScope( ScopeId parent, ScopeOwnerKind kind ) {
            return m_scopes.addScope(parent, kind);
        }

        void bindScope( NodeId nodeId, ScopeId scopeId ) {
            m_nodeScopes.emplace(nodeId, scopeId);
        }
        

        FileId getFileId() { return m_fileId; }

        size_t getTokenCount() { return m_tokens.count(); }

        void addToAST( Statement* statement) { m_ast.addStatement( statement ); }

        std::span<const Token> readTokens() { return m_tokens.getReadOnlyTokens(); }

        const std::vector<const Statement*>& readStatements() const { return m_ast.m_statements; };

        template<typename T, typename... Args>
        T* allocate(Args&&... args) {
            return m_arena.allocate<T>(std::forward<Args>(args)...);
        }

        void freeArena() { m_arena.reset(); }
    private:
        FileId m_fileId;
        ScopeId m_globalScope;
        ScopeId m_currentScope;

        Arena m_arena;
        TokenTable m_tokens;
        AST m_ast;
        SymbolTable m_symbols;
        TypeTable m_types;
        ScopeTable m_scopes;

        std::unordered_map<NodeId, SymbolId> m_resolvedSymbols;
        std::unordered_map<NodeId, TypeId> m_resolvedTypes;
        std::unordered_map<NodeId, ScopeId> m_nodeScopes;
};