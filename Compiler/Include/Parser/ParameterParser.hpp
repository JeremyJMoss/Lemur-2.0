#pragma once

#include <expected>
#include "Parser/TypeParser.hpp"
#include "AST/Parameter.hpp"
#include "Tokens/TokenStream.hpp"
#include "Driver/CompilationUnit.hpp"

class ParameterParser {
    public:
        ParameterParser( CompilationUnit& compUnit, TokenStream& tStream, TypeParser& typeParser )
            : m_compUnit( compUnit ), m_tokenStream( tStream ), m_typeParser( typeParser ) {}

        std::expected<std::vector<Parameter*>, ErrorVariant> parseFunctionParameters();

    private:
        CompilationUnit& m_compUnit;
        TokenStream& m_tokenStream;
        TypeParser& m_typeParser;

        std::expected<Parameter*, ErrorVariant> parseParameter();
};