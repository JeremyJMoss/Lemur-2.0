#pragma once

#include <string>

enum class OwnershipKind : std::uint8_t
{
    Owned,    // Exclusive ownership (like unique_ptr)
    Rref,     // Immutable borrow/reference
    Wref,     // Mutable borrow/reference
    Shared,   // Shared ownership (reference counted, non-thread-safe)
    Weak,     // Weak reference (non-owning observer to a shared pointer)
    None
};