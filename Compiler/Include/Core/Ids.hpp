#pragma once

/* === Dependencies === */

#include <limits>

/* === IDs === */

using NodeId        = std::size_t;
using SymbolId      = std::size_t;
using TypeId        = std::size_t;
using ScopeId       = std::size_t;
using FileId        = std::size_t;
using TokenId       = std::size_t;
using ModuleId      = std::size_t;
using OverloadSetId = std::size_t;

inline constexpr NodeId InvalidNodeId =
    std::numeric_limits<NodeId>::max();

inline constexpr SymbolId InvalidSymbolId =
    std::numeric_limits<SymbolId>::max();

inline constexpr TypeId InvalidTypeId =
    std::numeric_limits<TypeId>::max();

inline constexpr ScopeId InvalidScopeId =
    std::numeric_limits<ScopeId>::max();

inline constexpr FileId InvalidFileId =
    std::numeric_limits<FileId>::max();

inline constexpr TokenId InvalidTokenId =
    std::numeric_limits<TokenId>::max();

inline constexpr ModuleId InvalidModuleId =
    std::numeric_limits<ModuleId>::max();

inline constexpr OverloadSetId InvalidOverloadSetId =
    std::numeric_limits<OverloadSetId>::max();