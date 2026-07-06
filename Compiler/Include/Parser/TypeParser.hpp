#pragma once

/* === Imports ===*/
#include <expected>
#include <vector>
#include "Errors/Errors.hpp"

/* === Forward Declarations === */
struct ParsedType;

class CompilationUnit;
class Parser;
class TokenStream;

/* === Type Parser ===*/

class TypeParser{
    public:
        TypeParser( CompilationUnit& compUnit, TokenStream& tStream )
            :  m_compUnit( compUnit ), m_tokenStream( tStream ) {}
            
        std::expected<ParsedType*, ErrorVariant> parseType();

        std::expected<ParsedType*, ErrorVariant> parseNamedType();

        std::expected<std::vector<ParsedType*>, ErrorVariant> parseParameterTypes();

    private:
        CompilationUnit& m_compUnit;
        TokenStream& m_tokenStream;
};