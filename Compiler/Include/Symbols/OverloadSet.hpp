#pragma once

/* === Dependencies === */

#include <vector>
#include <string>

/* === Imports === */

#include "Core/Ids.hpp"

/* === Overload Set === */

struct OverloadSet {
    public:
        void addOverload( SymbolId symbolId );

        void setId( OverloadSetId overloadId );

        OverloadSetId id() const;

        const std::vector<SymbolId>& overloads() const;

    private:
        OverloadSetId m_id {};

        std::vector<SymbolId> m_overloads;
};