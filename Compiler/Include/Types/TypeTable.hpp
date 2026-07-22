#pragma once

/* === Dependencies === */

#include <vector>

/* === Imports === */

#include "Types/Type.hpp"
#include "Core/Ids.hpp"

/* === Type Table === */

class TypeTable {
    public:
        TypeId add( Type type );

        const Type& get( TypeId id ) const;
    private:
        std::vector<Type> types;
};