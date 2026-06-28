#pragma once

#include <string>

enum class OwnershipKind 
{
    Owned,    // Exclusive ownership (like unique_ptr)
    Rref,     // Immutable borrow/reference
    Wref,     // Mutable borrow/reference
    Shared,   // Shared ownership (reference counted, non-thread-safe)
    Weak,     // Weak reference (non-owning observer to a shared pointer)
    None
};

inline const std::string toString( const OwnershipKind kind ) 
{
    switch ( kind ) 
    {
        case OwnershipKind::Owned:       return "Exclusive Pointer";
        case OwnershipKind::Rref:        return "Immutable Reference";
        case OwnershipKind::Wref:        return "Mutable Reference";
        case OwnershipKind::Shared:      return "Shareable Pointer";
        case OwnershipKind::Weak:        return "Weak Pointer";
        case OwnershipKind::None:        return "No Ownership";
        default:                         return "Unknown";
    }
};