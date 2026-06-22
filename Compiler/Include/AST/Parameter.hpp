#pragma once

/* === Imports === */

#include <string>
#include <memory>
#include "AST/ParsedType.hpp"
#include "SourceControl/SourceLocation.hpp"

/* === Parameter === */

struct Parameter 
{
    SourceRange location;
    std::string name;
    std::unique_ptr<ParsedType> paramType;

    explicit Parameter( const std::string name, std::unique_ptr<ParsedType>&& type ) 
        : name( std::move( name ) ), paramType( std::move( type ) ) {}
};