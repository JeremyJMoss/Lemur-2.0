#pragma once

/* === Dependencies === */

#include <unordered_set>

/* === Imports === */

#include "Symbols/SymbolTable.hpp"
#include "Types/TypeTable.hpp"
#include "Scopes/ScopeTable.hpp"
#include "Modules/ModuleTable.hpp"
#include "Errors/Errors.hpp"

/* === Compiler Context === */

class CompilerContext
{
    public:
        
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

        SymbolTable m_symbols;
        TypeTable m_types;
        ScopeTable m_scopes;
        ModuleTable m_modules;

        ScopeId builtinScope;
};