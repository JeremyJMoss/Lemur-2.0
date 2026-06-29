#pragma once

#include <expected>
#include "Errors/ErrorReporter.hpp"
#include "AST/ParsedType.hpp"
#include "Tokens/TokenStream.hpp"

class CompilationUnit;
class Parser;

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