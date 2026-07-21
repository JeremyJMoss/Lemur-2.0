#pragma once

/* === Dependencies ===*/

#include <vector>
#include <unordered_map>
#include <span>
#include <string>

/* === Imports === */

#include "Symbols/Symbol.hpp"
#include "Semantics/NodeSemantics.hpp"
#include "Tokens/TokenTable.hpp"
#include "DataStructures/Arena.hpp"
#include "Modules/ModuleHeader.hpp"
#include "Core/Ids.hpp"

/* === Forward Declarations === */

struct Statement;
class CompilerContext;

/* === Compilation Unit === */

class CompilationUnit {
    public:
        CompilationUnit( const ModuleHeader& moduleHeader, CompilerContext& ctx ) 
            : m_moduleHeader( moduleHeader ), m_ctx( ctx ) {}

        TokenId addToken( Token token ) {
            return m_tokens.add( std::move( token ) );
        }

        void bindSymbol( NodeId nodeId, SymbolId symbolId ) {
            m_semanticInfo.bindSymbol( nodeId, symbolId );
        }

        void bindType( NodeId nodeId, TypeId typeId ) {
            m_semanticInfo.bindType( nodeId, typeId );
        }

        void bindScope( NodeId nodeId, ScopeId scopeId ) {
            m_semanticInfo.bindScope( nodeId, scopeId );
        }
        
        FileId getFileId() const { return m_moduleHeader.fileId; }

        std::string_view getModuleName() const { return m_moduleHeader.name; }

        size_t getTokenCount() const { return m_tokens.count(); }

        void addToAST( Statement* statement) { m_ast.addStatement( statement ); }

        std::span<const Token> readTokens() const { return m_tokens.getReadOnlyTokens(); }

        const AST& ast() { return m_ast; }

        template<typename T, typename... Args>
        T* allocate( Args&&... args ) {
            return m_arena.allocate<T>( std::forward<Args>( args )... );
        }

        void freeArena() { m_arena.reset(); }

        CompilerContext& context() { return m_ctx; }
    private:
        const ModuleHeader& m_moduleHeader;
        CompilerContext& m_ctx;

        Arena m_arena;
        TokenTable m_tokens;
        AST m_ast;

        NodeSemantics m_semanticInfo;
};