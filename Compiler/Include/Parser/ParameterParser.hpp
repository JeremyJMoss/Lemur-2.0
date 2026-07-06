#pragma once

/* === Imports === */
#include <expected>
#include <vector>
#include "Errors/Errors.hpp"

/* === Forward Declarations === */
struct Parameter;
class CompilationUnit;
class TokenStream;
class TypeParser;

/* === Parameter Parser === */
class ParameterParser {
    public:
        ParameterParser( 
            CompilationUnit& compUnit, 
            TokenStream& tStream, 
            TypeParser& typeParser 
        ) : m_compUnit( compUnit ), 
            m_tokenStream( tStream ), 
            m_typeParser( typeParser ) {}

        std::expected<std::vector<Parameter*>, ErrorVariant> parseFunctionParameters();

    private:
        CompilationUnit& m_compUnit;
        TokenStream& m_tokenStream;
        TypeParser& m_typeParser;

        std::expected<Parameter*, ErrorVariant> parseParameter();
};