#pragma once

#include <expected>
#include <memory>
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

class Parser;

class ExpressionParser{
    public:
        ExpressionParser( 
            TokenStream& tStream,
            TypeParser& typeParser,
            ParameterParser& paramParser
        ) 
        : m_tokenStream( tStream ), m_typeParser( typeParser ), m_paramParser( paramParser ) {}

        void setStatementParser( StatementParser* stmtParser ) { m_stmtParser = stmtParser; }

        std::expected<std::unique_ptr<Expression>, ErrorVariant> parseExpression();

        std::expected<std::unique_ptr<Expression>, ErrorVariant> parsePostFixExpression();

        std::expected<std::unique_ptr<Assignment>, ErrorVariant> parseAssignment();

        std::expected<std::unique_ptr<Expression>, ErrorVariant> parseInitialiser();

    private:
        static const std::regex s_RE_STRING_REPL;

        TokenStream& m_tokenStream;
        TypeParser& m_typeParser;
        ParameterParser& m_paramParser;
        StatementParser* m_stmtParser = nullptr;
        std::size_t m_precedence = 0;

        std::expected<LiteralValue, ErrorVariant> getLiteralValue();

        std::expected<std::unique_ptr<Expression>, ErrorVariant> parseUnary();

        std::expected<std::unique_ptr<Range>, ErrorVariant> parseRange( std::unique_ptr<Expression>&& start );

        std::expected<std::unique_ptr<Expression>, ErrorVariant> parsePrimaryLiteral();

        std::expected<std::vector<std::unique_ptr<Expression>>, ErrorVariant> parseFunctionCallArgs();

        std::expected<std::unique_ptr<FunctionCall>, ErrorVariant> parseFunctionCall();

        std::expected<std::unique_ptr<FunctionLiteral>, ErrorVariant> parseFunctionLiteral();

        std::size_t getPrecedence( TokenSymbol op );
};