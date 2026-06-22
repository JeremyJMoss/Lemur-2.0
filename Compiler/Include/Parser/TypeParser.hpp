#pragma once

#include <expected>
#include <memory>
#include "Parser/ParserUtils.hpp"
#include "Errors/ErrorReporter.hpp"
#include "AST/ParsedType.hpp"

class Parser;

class TypeParser{
    public:
        TypeParser( ParserUtils& utils )
            :  m_utils( utils ) {}
            
        std::expected<std::unique_ptr<ParsedType>, ErrorVariant> parseType();

        std::expected<std::vector<std::unique_ptr<ParsedType>>, ErrorVariant> parseParameterTypes();

    private:
        ParserUtils& m_utils;
};