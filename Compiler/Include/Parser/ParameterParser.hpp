#pragma once

#include "Parser/TypeParser.hpp"
#include "AST/Parameter.hpp"

class ParameterParser {
    public:
        ParameterParser( ParserUtils& utils, TypeParser& typeParser )
            : m_utils( utils ), m_typeParser( typeParser ) {}

        std::expected<std::vector<std::unique_ptr<Parameter>>, ErrorVariant> parseFunctionParameters();

    private:
        ParserUtils& m_utils;
        TypeParser& m_typeParser;

        std::expected<std::unique_ptr<Parameter>, ErrorVariant> parseParameter();
};