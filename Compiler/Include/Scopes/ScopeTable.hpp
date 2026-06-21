#include <vector>
#include "Scopes/Scope.hpp"

class ScopeTable {
    public:
        ScopeId addScope(
            ScopeId parentId,
            ScopeOwnerKind kind
        ) {
            ScopeId id = m_scopes.size();

            m_scopes.emplace_back(id, parentId, kind);

            return id;
        }

    private:
        std::vector<Scope> m_scopes;
};