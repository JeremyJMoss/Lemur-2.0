#pragma once

/* === Dependencies === */

#include <limits>
#include <compare>

/* === Ids === */

template<typename Tag>
struct Id
{
    using ValueType = std::size_t;

    static constexpr ValueType Invalid =
        std::numeric_limits<ValueType>::max();

    ValueType value = Invalid;

    constexpr bool valid() const
    {
        return value != Invalid;
    }

    auto operator<=>(const Id&) const = default;
};

namespace std
{
    template<typename Tag>
   struct hash<Id<Tag>>
    {
        size_t operator()(const Id<Tag>& id) const noexcept
        {
            return std::hash<size_t>{}(id.value);
        }
    };
}

struct NodeTag {};
struct SymbolTag {};
struct TypeTag {};
struct ScopeTag {};
struct FileTag {};
struct TokenTag {};
struct ModuleTag {};
struct OverloadSetTag {};

using NodeId        = Id<NodeTag>;
using SymbolId      = Id<SymbolTag>;
using TypeId        = Id<TypeTag>;
using ScopeId       = Id<ScopeTag>;
using FileId        = Id<FileTag>;
using TokenId       = Id<TokenTag>;
using ModuleId      = Id<ModuleTag>;
using OverloadSetId = Id<OverloadSetTag>;