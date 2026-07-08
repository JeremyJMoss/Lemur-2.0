#pragma once

/* === Imports === */

#include <vector>
#include <span>
#include <unordered_map>
#include <expected>
#include "Tokens/Token.hpp"
#include "Errors/Errors.hpp"

using ModuleId = std::size_t;
using FileId = std::size_t;

/* === Module Info === */

struct ModuleInfo
{
    ModuleId id;
    FileId fileId;
    std::string name;
};

/* === Module Table === */

class ModuleTable
{
    public:
        bool add( FileId fileId, std::string moduleName ) 
        {
            ModuleId id = m_modules.size();
            
            m_modules.emplace_back( id, fileId, std::move( moduleName ) );

            auto [it, inserted] = m_lookup.emplace( moduleName, id );

            return inserted;
        }

        ModuleInfo& get( ModuleId id ) 
        {
            return m_modules[id];
        }

        const ModuleInfo& get( ModuleId id ) const 
        {
            return m_modules[id];
        }

        size_t count() const 
        { 
            return m_modules.size(); 
        }

        std::expected<ModuleInfo, Diagnostic> find( std::string_view name ) const {
            auto it = m_lookup.find( std::string( name ) );

            if ( it != m_lookup.end() ) 
            {   
                return m_modules[it->second];
            }

            return std::unexpected( 
                Diagnostic(
                    "Could not find module name in module map lookup",
                    ErrorCategory::Linking,
                    ErrorSeverity::Fatal
                ) 
            );
        }


        ModuleTable() {}

    private:
        std::vector<ModuleInfo> m_modules;
        std::unordered_map<std::string, ModuleId> m_lookup;
};