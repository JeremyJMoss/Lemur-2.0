#pragma once

/* === Dependencies ===*/

#include <vector>
#include <span>
#include <unordered_map>
#include <expected>
#include <unordered_set>

/* === Imports === */

#include "Errors/Errors.hpp"
#include "Modules/ModuleHeader.hpp"
#include "Core/Ids.hpp"

/* === Module Table === */

class ModuleTable
{
    public:
        bool add( FileId fileId, std::string moduleName, std::vector<ImportDirective> imports );

        ModuleHeader& get( ModuleId id );

        const ModuleHeader& get( ModuleId id ) const;

        size_t count() const;

        const ModuleHeader* find( std::string_view name ) const;

        std::expected<std::unordered_set<ModuleId>, Diagnostic> resolveImports( ModuleId entry );

        std::vector<ModuleId> buildParseOrder( ModuleId entry );

    private:
        std::vector<ModuleHeader> m_modules;
        std::unordered_map<std::string, ModuleId> m_lookup;

        void buildParseOrder( ModuleId id, std::unordered_set<ModuleId>& visited, std::vector<ModuleId>& order );
};