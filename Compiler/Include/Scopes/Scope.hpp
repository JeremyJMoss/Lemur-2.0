#include "Scopes/NameBinding.hpp"

using ScopeId = size_t;
constexpr ScopeId InvalidScope = static_cast<ScopeId>(-1);

enum class ScopeOwnerKind 
{
    Global,
    Function,
    Lambda,
    Block
};

struct Scope {
    ScopeId m_id;
    ScopeId m_parentId;
    ScopeOwnerKind m_kind;

    bool hasParent(const Scope& scope) {
        return scope.m_parentId != InvalidScope;
    }

    Scope( ScopeId id, ScopeId parentId, ScopeOwnerKind kind )
        : m_id( id ), m_parentId( parentId ), m_kind( kind ) {}
};