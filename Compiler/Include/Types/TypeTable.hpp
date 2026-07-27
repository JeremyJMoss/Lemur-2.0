#pragma once

/* === Dependencies === */

#include <vector>
#include <span>

/* === Imports === */

#include "Types/Type.hpp"
#include "Core/Ids.hpp"

/* === Type Table === */

class TypeTable {
    public:
        TypeId add( Type* type );

        Type& get( TypeId id );

        const Type& get( TypeId id ) const;

        std::span<const Type* const> getAll() const;
    private:
        std::vector<Type*> types;
};