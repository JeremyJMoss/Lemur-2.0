#pragma once

/* === Imports === */

#include <vector>
#include <span>
#include <unordered_map>
#include <expected>
#include "Tokens/Token.hpp"
#include "Modules/ModuleHeader.hpp"

using ModuleId = std::size_t;
using FileId = std::size_t;

/* === Module Table === */

class ModuleTable
{
    public:
        bool add(FileId fileId, std::string moduleName)
        {
            ModuleId id = m_modules.size();

            auto [it, inserted] = m_lookup.emplace(moduleName, id);

            if (!inserted)
                return false;

            m_modules.emplace_back( id, fileId, std::move( moduleName ) );

            return true;
        }

        ModuleHeader& get( ModuleId id ) 
        {
            return m_modules[id];
        }

        const ModuleHeader& get( ModuleId id ) const 
        {
            return m_modules[id];
        }

        size_t count() const 
        { 
            return m_modules.size(); 
        }

        const ModuleHeader* find( std::string_view name ) const {
            auto it = m_lookup.find( std::string( name ) );

            if ( it != m_lookup.end() ) 
            {   
                return &m_modules[it->second];
            }

            return nullptr;
        }

    private:
        std::vector<ModuleHeader> m_modules;
        std::unordered_map<std::string, ModuleId> m_lookup;
};