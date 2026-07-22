/* === Main Import === */

#include "Types/TypeTable.hpp"

/* === Type Table Methods === */

TypeId TypeTable::add( Type type ) 
{
    TypeId id = types.size();

    type.setId( id );

    types.push_back( std::move( type ) );

    return id;
}

const Type& TypeTable::get( TypeId id ) const 
{
    return types[id];
}