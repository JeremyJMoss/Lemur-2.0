#pragma once

/* === Dependencies === */

#include <vector>

/* === Imports === */

#include "Symbols/OverloadSet.hpp"
#include "Core/Ids.hpp"

/* === Overload Set Table === */

class OverloadSetTable
{
    public:
        OverloadSetId add( OverloadSet* overloadSet );

        OverloadSet& get( OverloadSetId id );

        const OverloadSet& get( OverloadSetId id ) const;

        OverloadSetTable() {}

    private:
        std::vector<OverloadSet*> overloadSets;
};