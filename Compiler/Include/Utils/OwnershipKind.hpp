#pragma once

enum class OwnershipKind : std::uint8_t
{
    Owned,    // Exclusive ownership (like unique_ptr)
    Rref,     // Immutable borrow/reference
    Wref,     // Mutable borrow/reference
    Shared,   // Shared ownership (reference counted, non-thread-safe)
    Weak,     // Weak reference (non-owning observer to a shared pointer)
    None
};

inline std::string toString ( const OwnershipKind owner )
{
    switch ( owner )
    {
        case OwnershipKind::Owned:  return "Owned Pointer";
        case OwnershipKind::Rref:   return "Read Only Reference";
        case OwnershipKind::Wref:   return "Writeable Reference";
        case OwnershipKind::Shared: return "Shared Pointer";
        case OwnershipKind::Weak:   return "Weak Pointer";
        case OwnershipKind::None:   return "None";
        default:                    return "Unknown";
    }
}