#pragma once

/* === Dependencies === */

#include <unordered_set>
#include <format>

/* === Imports === */

#include "Symbols/SymbolTable.hpp"
#include "Types/TypeTable.hpp"
#include "Scopes/ScopeTable.hpp"
#include "Modules/ModuleTable.hpp"
#include "Errors/Errors.hpp"
#include "DataStructures/Arena.hpp"
#include "Errors/ErrorReporter.hpp"
#include "SourceControl/SourceManager.hpp"

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

        SymbolId addSymbol( Symbol symbol ) {
            return m_symbols.add( std::move( symbol ) );
        }

        TypeId addType( Type type ) {
            return m_types.add( std::move( type ) );
        }

        ScopeId createScope( ScopeId parent, ScopeOwnerKind kind ) 
        {
            return m_scopes.addScope( parent, kind );
        }

        std::expected<void, Diagnostic> declareInScope( ScopeId scopeId, std::string_view name, SymbolId symbolId )
        {
            auto result = m_scopes.declare( scopeId, name, symbolId );

            if (!result)
            {
                SymbolId previous = result.error();

                const Symbol& oldSymbol = m_symbols.get(previous);

                return std::unexpected(
                    Diagnostic(
                        std::format(
                            "Symbol '{}' already declared in current scope. Cannot redeclare symbol '{}'",
                            name, 
                            name
                        ),
                        ErrorCategory::Semantic,
                        ErrorSeverity::Error,
                        oldSymbol.declaration
                    )
                );
            }

            return {};
        }
        
        ModuleId addModule( FileId fileId, std::string moduleIdentifier, std::vector<ImportDirective> imports )
        {
            return m_modules.add( fileId, moduleIdentifier, std::move( imports ) );
        }

        const ModuleHeader& getModuleHeader( ModuleId moduleId ) { return m_modules.get( moduleId ); }

        const ModuleHeader* getModuleHeader( const std::string& moduleName ) { return m_modules.find( moduleName ); }

        std::expected<std::unordered_set<ModuleId>, Diagnostic> resolveModuleImports( ModuleId entryModuleId ) { 
            return m_modules.resolveImports( entryModuleId ); 
        }

        std::vector<ModuleId> buildModuleParseOrder( ModuleId entryModuleId ) { 
            return m_modules.buildParseOrder( entryModuleId ); 
        }

        std::size_t moduleCount() { return m_modules.count(); }

        ScopeId getBuiltInScope() const
        {
            return m_builtinScope;
        }

        template<typename T, typename... Args>
        T* allocate( Args&&... args ) {
            return m_globalArena.allocate<T>( std::forward<Args>( args )... );
        }

        void freeArena() { m_globalArena.reset(); }

        ErrorReporter& errors() { return m_errReporter; }

        SourceManager& source() { return m_sourceManager; }

        private:
            Arena m_globalArena;

            SourceManager m_sourceManager;
            ErrorReporter m_errReporter;
            SymbolTable m_symbols;
            TypeTable m_types;
            ScopeTable m_scopes;
            ModuleTable m_modules;

            ScopeId m_builtinScope;
};