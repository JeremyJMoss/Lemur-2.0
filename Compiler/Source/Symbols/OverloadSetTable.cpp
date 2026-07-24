/* === Dependencies ===*/

#include "Symbols/OverloadSetTable.hpp"

/* === Symbol Table Methods === */

OverloadSetId OverloadSetTable::add( OverloadSet* overloadSet ) {
    OverloadSetId id{ overloadSets.size() };

    overloadSet->setId( id );

    overloadSets.push_back( std::move( overloadSet ) );

    return id;
}

OverloadSet& OverloadSetTable::get( OverloadSetId id ) {
    return *overloadSets[id.value];
}

const OverloadSet& OverloadSetTable::get( OverloadSetId id ) const {
    return *overloadSets[id.value];
}

