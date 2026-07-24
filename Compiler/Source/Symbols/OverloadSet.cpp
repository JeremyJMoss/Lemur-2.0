/* === Main Import === */

#include "Symbols/OverloadSet.hpp"

/* === Overload Set Methods === */

void OverloadSet::addOverload( SymbolId symbolId )
{
    m_overloads.push_back( symbolId );
}

OverloadSetId OverloadSet::id() const
{
    return m_id;
}

void OverloadSet::setId( OverloadSetId overloadId )
{
    m_id = overloadId;
}

const std::vector<SymbolId>& OverloadSet::overloads() const
{
    return m_overloads;
}