#pragma once

#include <string>

enum class OwnershipKind 
{
    Own,    // Exclusive ownership (like unique_ptr)
    View,   // Immutable borrow/reference
    Mut,    // Mutable borrow/reference
    Share,  // Shared ownership (reference counted, non-thread-safe)
    Atomic, // Shared ownership with thread-safe atomic refcount
    Weak,    // Weak reference (non-owning observer to a shared pointer)
    None
};

inline const std::string toString( const OwnershipKind kind ) 
{
    switch ( kind ) 
    {
        case OwnershipKind::Own:         return "Exclusive Pointer";
        case OwnershipKind::View:        return "Immutable Reference";
        case OwnershipKind::Mut:         return "Mutable Reference";
        case OwnershipKind::Share:       return "Shareable Pointer";
        case OwnershipKind::Atomic:      return "Thread Safe Shared Pointer";
        case OwnershipKind::Weak:        return "Weak Pointer";
        case OwnershipKind::None:        return "No Ownership";
        default:                         return "Unknown";
    }
};