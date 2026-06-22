#pragma once

#include "Parser/TypeParser.hpp"
#include "AST/Parameter.hpp"
#include "Tokens/TokenStream.hpp"

class ParameterParser {
    public:
        ParameterParser( TokenStream& tStream, ParserUtils& utils, TypeParser& typeParser )
            : m_tokenStream( tStream ), m_utils( utils ), m_typeParser( typeParser ) {}

        std::expected<std::vector<std::unique_ptr<Parameter>>, ErrorVariant> parseFunctionParameters();

    private:
        TokenStream& m_tokenStream;
        ParserUtils& m_utils;
        TypeParser& m_typeParser;

        std::expected<std::unique_ptr<Parameter>, ErrorVariant> parseParameter();
};