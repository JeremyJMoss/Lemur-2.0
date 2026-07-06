#pragma once

/* === Imports === */

#include <iostream>
#include <expected>
#include "Errors/Errors.hpp"
#include "AST/ParsedType.hpp"
#include "Parser/TypeParser.hpp"
#include "Parser/ExpressionParser.hpp"
#include "Parser/StatementParser.hpp"
#include "Parser/ParameterParser.hpp"
#include "Tokens/TokenStream.hpp"

/* === Forward Declarations === */

struct Token;
struct Statement;

class ErrorReporter;
class CompilationUnit;

/* === Parser === */

class Parser 
{
    public:
        Parser(CompilationUnit& compUnit, ErrorReporter& errReporter);
        
        void parse();

        std::expected<Statement*, ErrorVariant> createStatement( const Token& token );

    private:
        CompilationUnit& m_compUnit;
        ErrorReporter& m_errReporter;
        TokenStream m_tokenStream;
        TypeParser m_typeParser;
        ParameterParser m_paramParser;
        StatementParser m_stmtParser;
        ExpressionParser m_exprParser;

        void parseNextStatement();

        std::expected<Statement*, ErrorVariant> parseKeywordStatement( const Token& token );

        std::expected<Statement*, ErrorVariant> parseIdentifierStatement( const Token& token );

        std::expected<Statement*, ErrorVariant> parseExpressionStatement( const Token& token );
};
