#pragma once

#include <expected>
#include <regex>
#include "AST/ASTNode.hpp"
#include "AST/Assignment.hpp"
#include "AST/Range.hpp"
#include "AST/FunctionLiteral.hpp"
#include "AST/FunctionCall.hpp"
#include "AST/Literal.hpp"
#include "Errors/ErrorReporter.hpp"
#include "Parser/TypeParser.hpp"
#include "Parser/ParameterParser.hpp"
#include "Parser/StatementParser.hpp"
#include "Tokens/TokenStream.hpp"
#include "Driver/CompilationUnit.hpp"

class Parser;

class ExpressionParser{
    public:
        ExpressionParser(
            CompilationUnit& compUnit,
            TokenStream& tStream,
            TypeParser& typeParser,
            ParameterParser& paramParser
        ) 
        : m_compUnit( compUnit ), 
        m_tokenStream( tStream ), 
        m_typeParser( typeParser ), 
        m_paramParser( paramParser ) {}

        void setStatementParser( StatementParser* stmtParser ) { m_stmtParser = stmtParser; }

        std::expected<Expression*, ErrorVariant> parseExpression( std::size_t min_precedence = 0);

        std::expected<Expression*, ErrorVariant> parsePostFixExpression();

        std::expected<Assignment*, ErrorVariant> parseAssignment();

        std::expected<Expression*, ErrorVariant> parseInitialiser();

    private:
        static const std::regex s_RE_STRING_REPL;

        CompilationUnit& m_compUnit;
        TokenStream& m_tokenStream;
        TypeParser& m_typeParser;
        ParameterParser& m_paramParser;
        StatementParser* m_stmtParser = nullptr;

        std::expected<LiteralValue, ErrorVariant> getLiteralValue();

        std::expected<Expression*, ErrorVariant> parseUnary();

        std::expected<Range*, ErrorVariant> parseRange( Expression* start );

        std::expected<Expression*, ErrorVariant> parsePrimaryLiteral();

        std::expected<std::vector<Expression*>, ErrorVariant> parseFunctionCallArgs();

        std::expected<FunctionCall*, ErrorVariant> parseFunctionCall();

        std::expected<FunctionLiteral*, ErrorVariant> parseFunctionLiteral();

        std::size_t getPrecedence( TokenSymbol op );
};