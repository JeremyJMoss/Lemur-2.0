#pragma once

/* === Imports === */

#include <vector>

using ModuleId = std::size_t;
using Dependencies = std::vector<ModuleId>;

/* === Dependency Graph === */

class DependencyGraph {
    public:
        void addModule()
        {
            m_moduleDependencies.emplace_back();
        }

        void addDependency( ModuleId importingModule, ModuleId import )
        {
            m_moduleDependencies[importingModule].emplace_back(import);
        }

    private:
        std::vector<Dependencies> m_moduleDependencies;
};