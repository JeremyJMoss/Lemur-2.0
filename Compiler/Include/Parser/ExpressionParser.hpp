#pragma once

/* === Imports === */

#include <expected>
#include <vector>
#include <string>
#include <variant>
#include "Errors/Errors.hpp"

/* === Forward Declarations === */

struct ASTNode;
struct Assignment;
struct Range;
struct FunctionLiteral;
struct FunctionCall;
struct Literal;

class Parser;
class CompilationUnit;
class TokenStream;
class TypeParser;
class ParameterParser;
class StatementParser;

using LiteralValue = std::variant<
    std::string_view, 
    char, 
    int, 
    float, 
    bool, 
    std::monostate
>;

/* === Expression Statement === */

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

        std::expected<Expression*, ErrorVariant> parseExpression( const std::size_t min_precedence = 0 );

        std::expected<Expression*, ErrorVariant> parsePostFixExpression();

        std::expected<Assignment*, ErrorVariant> parseAssignment();

        std::expected<Expression*, ErrorVariant> parseInitialiser();

    private:
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

        /// @brief Gets operator precedence
        /// 
        /// @param op Operator to check precedence of
        /// @returns The precedence number of the operator passed in
        std::size_t getPrecedence( const TokenSymbol op );

        std::string unescapeString( std::string_view raw );
};