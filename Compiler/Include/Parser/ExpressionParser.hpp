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
    std::string, 
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

        std::expected<Expression*, Diagnostic> parseExpression( const std::size_t min_precedence = 0 );

        std::expected<Expression*, Diagnostic> parsePostFixExpression();

        std::expected<Assignment*, Diagnostic> parseAssignment();

        std::expected<Expression*, Diagnostic> parseInitialiser();

    private:
        CompilationUnit& m_compUnit;
        TokenStream& m_tokenStream;
        TypeParser& m_typeParser;
        ParameterParser& m_paramParser;
        StatementParser* m_stmtParser = nullptr;

        std::expected<LiteralValue, Diagnostic> getLiteralValue();

        std::expected<Expression*, Diagnostic> parseUnary();

        std::expected<Range*, Diagnostic> parseRange( Expression* start );

        std::expected<Expression*, Diagnostic> parsePrimaryLiteral();

        std::expected<std::vector<Expression*>, Diagnostic> parseFunctionCallArgs();

        std::expected<FunctionCall*, Diagnostic> parseFunctionCall();

        std::expected<FunctionLiteral*, Diagnostic> parseFunctionLiteral();

        /// @brief Gets operator precedence
        /// 
        /// @param op Operator to check precedence of
        /// @returns The precedence number of the operator passed in
        std::size_t getPrecedence( const TokenSymbol op );

        std::string unescapeString( std::string_view raw );
};