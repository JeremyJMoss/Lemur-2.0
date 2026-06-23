#pragma once

#include <expected>
#include <memory>
#include "Errors/ErrorReporter.hpp"
#include "AST/ParsedType.hpp"
#include "Tokens/TokenStream.hpp"

class Parser;

class TypeParser{
    public:
        TypeParser( TokenStream& tStream )
            :  m_tokenStream( tStream ) {}
            
        std::expected<std::unique_ptr<ParsedType>, ErrorVariant> parseType();

        std::expected<std::unique_ptr<ParsedType>, ErrorVariant> parseNamedType();

        std::expected<std::vector<std::unique_ptr<ParsedType>>, ErrorVariant> parseParameterTypes();

    private:
        TokenStream& m_tokenStream;
};