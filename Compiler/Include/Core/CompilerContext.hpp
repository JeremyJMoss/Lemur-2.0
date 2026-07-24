#pragma once

/* === Dependencies === */

#include <unordered_set>
#include <format>

/* === Imports === */

#include "Symbols/SymbolTable.hpp"
#include "Types/TypeTable.hpp"
#include "Scopes/ScopeTable.hpp"
#include "Scopes/ScopeStack.hpp"
#include "Modules/ModuleTable.hpp"
#include "Symbols/OverloadSetTable.hpp"
#include "Errors/Errors.hpp"
#include "DataStructures/Arena.hpp"
#include "Errors/ErrorReporter.hpp"
#include "SourceControl/SourceManager.hpp"
#include "Semantics/NodeSemantics.hpp"

/* === Forward Declarations === */

class BuiltInRegistry;

/* === Compiler Context === */

class CompilerContext
{
    public:
        CompilerContext()
            : m_sourceManager(),
              m_errReporter( m_sourceManager ) {}

        void initialize();

        std::expected<void, Diagnostic> declareInScope( ScopeId scopeId, std::string_view name, SymbolId symbolId );

        ScopeId getBuiltInScope() const
        {
            return m_builtinScope;
        }

        template<typename T, typename... Args>
        T* allocate( Args&&... args ) {
            return m_globalArena.allocate<T>( std::forward<Args>( args )... );
        }

        void freeArena() { m_globalArena.reset(); }

        SymbolTable& symbols() { return m_symbols; }

        TypeTable& types() { return m_types; }

        ScopeTable& scopes() { return m_scopes; }

        ModuleTable& modules() { return m_modules; }

        ErrorReporter& errors() { return m_errReporter; }

        SourceManager& source() { return m_sourceManager; }

        NodeSemantics& nodeSemantics() { return m_semanticInfo; }

        OverloadSetTable& overloads() { return m_overloadSets; };

        ScopeId enterScope( NodeId owner, ScopeOwnerKind kind );

        void leaveScope();

        ScopeId currentScope() const;

    private:
        Arena m_globalArena;

        SourceManager m_sourceManager;
        ErrorReporter m_errReporter;
        SymbolTable m_symbols;
        TypeTable m_types;
        ScopeTable m_scopes;
        ModuleTable m_modules;
        OverloadSetTable m_overloadSets;

        ScopeStack m_scopeStack;
        ScopeId m_builtinScope{};
        NodeSemantics m_semanticInfo;
};