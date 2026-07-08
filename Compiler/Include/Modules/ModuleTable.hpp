#pragma once

/* === Imports === */

#include <vector>
#include <span>
#include <unordered_map>
#include <expected>
#include <vector>
#include "Tokens/Token.hpp"

using ModuleId = std::size_t;
using FileId = std::size_t;

/* === Module Info === */

struct ModuleInfo
{
    ModuleId id;
    FileId fileId;
    std::string name;
    std::vector<ModuleId> imports;
};

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

        const ModuleInfo* find( std::string_view name ) const {
            auto it = m_lookup.find( std::string( name ) );

            if ( it != m_lookup.end() ) 
            {   
                return &m_modules[it->second];
            }

            return nullptr;
        }

    private:
        std::vector<ModuleInfo> m_modules;
        std::unordered_map<std::string, ModuleId> m_lookup;
};