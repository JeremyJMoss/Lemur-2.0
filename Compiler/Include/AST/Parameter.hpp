#pragma once

/* === Imports === */

#include <string>
#include <memory>
#include "AST/ParsedType.hpp"
#include "Utils/SourceLocation.hpp"
#include "Types/TypeInfo.hpp"

/* === Parameter === */

struct Parameter 
{
    SourceRange location;
    std::string name;
    std::unique_ptr<ParsedType> paramType;
    std::shared_ptr<TypeInfo> resolvedType;

    explicit Parameter( const std::string& name, std::unique_ptr<ParsedType>&& type ) 
        : name( std::move( name ) ), paramType( std::move( type ) ) {}
};