/* === Main Import === */

#include "Modules/ModuleTable.hpp"

/* === Dependencies === */

#include <expected>
#include <queue>
#include <unordered_set>
#include <format>
#include <vector>

/* === Imports === */

#include "Logging/Logger.hpp"
#include "Core/Ids.hpp"
#include "Errors/Errors.hpp"

/* === Module Table Methods === */

ModuleInfo& ModuleTable::get( ModuleId id ) 
{ 
    return m_modules[id.value]; 
}

const ModuleInfo& ModuleTable::get( ModuleId id ) const 
{ 
    return m_modules[id.value]; 
}

size_t ModuleTable::count() const 
{ 
    return m_modules.size(); 
}

bool ModuleTable::add( FileId fileId, std::string moduleName, std::vector<ImportDirective> imports )
{
    ModuleId id{ m_modules.size() };


    auto [it, inserted] = m_lookup.emplace( moduleName, id );

    if ( !inserted ) return false;

    m_modules.emplace_back( ModuleHeader(id, fileId, std::move( moduleName ), std::move( imports )) );

    return true;
}

const ModuleInfo* ModuleTable::find( std::string_view name ) const 
{
    auto it = m_lookup.find( std::string( name ) );

    if ( it != m_lookup.end() ) 
    {   
        return &m_modules[it->second.value];
    }

    return nullptr;
}

std::expected<std::unordered_set<ModuleId>, Diagnostic> ModuleTable::resolveImports( ModuleId entry ) 
{
    Logger::trace( "Resolving Imports for Module" );

    std::queue<ModuleId> pending;
    std::unordered_set<ModuleId> visited;

    pending.push( entry );
    visited.insert( entry );

    while( !pending.empty() ) 
    {
        ModuleId current = pending.front();
        pending.pop();

        ModuleInfo& module = this->get( current );

        for ( ImportDirective& import : module.header.imports )
        {
            const ModuleInfo* imported = this->find( import.moduleName );

            if ( imported == nullptr )
            {
                return std::unexpected(
                    Diagnostic(
                        std::format(
                            "Importing module '{}' into module '{}' failed. Unable to resolve module '{}'",
                            import.moduleName,
                            module.header.name,
                            import.moduleName
                        ),
                        ErrorCategory::Linking,
                        ErrorSeverity::Error
                    )
                );
            }

            import.moduleId = imported->header.id;

            if ( visited.insert(imported->header.id).second )
            {
                pending.push( imported->header.id );
            }
        }
    }

    return visited;
}

std::vector<ModuleId> ModuleTable::buildParseOrder( ModuleId entry ) 
{
    std::vector<ModuleId> order;
    std::unordered_set<ModuleId> visited;

    ModuleInfo entryModule = this->get( entry );

    for( ImportDirective import : entryModule.header.imports )
    {
        if( !import.moduleId.has_value() ) {
            throw InternalCompilerError( "Unexpected module id missing.\nPlease report this bug.");
        }

        buildParseOrder( import.moduleId.value(), visited, order );
    }

    order.push_back( entry );

    return order;
}

void ModuleTable::buildParseOrder( ModuleId id, std::unordered_set<ModuleId>& visited, std::vector<ModuleId>& order )
{
    if ( !visited.insert(id).second ) return;

    ModuleInfo& module = get( id );

    for ( ImportDirective& import : module.header.imports ) {
        if( !import.moduleId.has_value() ) {
            throw InternalCompilerError( "Unexpected module id missing.\nPlease report this bug.");
        }

        buildParseOrder( import.moduleId.value(), visited, order );
    }

    order.push_back( id );
}