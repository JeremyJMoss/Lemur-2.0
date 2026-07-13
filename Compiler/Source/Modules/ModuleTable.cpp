#include "Modules/ModuleTable.hpp"

#include <queue>
#include <cassert>

using ModuleId = std::size_t;

std::expected<std::unordered_set<ModuleId>, Diagnostic> ModuleTable::resolveImports( ModuleId entry ) 
{
    std::queue<ModuleId> pending;
    std::unordered_set<ModuleId> visited;

    pending.push( entry );
    visited.insert( entry );

    while( !pending.empty() ) 
    {
        ModuleId current = pending.front();
        pending.pop();

        ModuleHeader& module = this->get( current );

        for ( ImportDirective& import : module.imports )
        {
            const ModuleHeader* imported = this->find( import.moduleName );

            if ( !imported )
            {
                return std::unexpected(
                    Diagnostic(
                        "Imported Module could not be found in module resolution tree",
                        ErrorCategory::Linking,
                        ErrorSeverity::Error
                    )
                );
            }

            import.moduleId = imported->id;

            if ( visited.insert(imported->id).second )
            {
                pending.push( imported->id );
            }
        }
    }

    return visited;
}

std::vector<ModuleId> ModuleTable::buildParseOrder( ModuleId entry ) 
{
    std::vector<ModuleId> order;
    std::unordered_set<ModuleId> visited;

    ModuleHeader entryModuleHeader = this->get( entry );

    for( ImportDirective import : entryModuleHeader.imports )
    {
        assert( import.moduleId.has_value() );

        buildParseOrder( import.moduleId.value(), visited, order );
    }

    return order;
}

void ModuleTable::buildParseOrder( ModuleId id, std::unordered_set<ModuleId>& visited, std::vector<ModuleId>& order )
{
    if ( !visited.insert(id).second ) return;

    ModuleHeader& module = get( id );

    for ( ImportDirective& import : module.imports ) {
        assert( import.moduleId.has_value());

        buildParseOrder( import.moduleId.value(), visited, order );
    }

    order.push_back( id );
}