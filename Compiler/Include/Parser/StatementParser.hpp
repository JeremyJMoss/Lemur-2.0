#pragma once

#include "Parser/ParameterParser.hpp"
#include "AST/FunctionDeclaration.hpp"
#include "AST/Block.hpp"
#include "AST/Return.hpp"
#include "AST/IfConditional.hpp"
#include "AST/ForLoop.hpp"
#include "AST/VariableDeclaration.hpp"

/* === Forward Declarations === */

class Parser;
class ExpressionParser;

/* === StatementParser === */

class StatementParser 
{
    public:
        StatementParser( 
            Parser& parent, 
            ParserUtils& utils, 
            ErrorReporter& errReporter,
            TypeParser& typeParser,
            ParameterParser& paramParser
        ) 
        : m_parent( parent ), m_utils( utils ), 
        m_errReporter( errReporter ), m_typeParser( typeParser ), 
        m_paramParser( paramParser ) {}

        void setExpressionParser( ExpressionParser* exprParser ) { m_exprParser = exprParser; }

        std::expected<std::unique_ptr<FunctionDeclaration>, ErrorVariant> parseFunctionDeclaration();

        std::expected<std::unique_ptr<Block>, ErrorVariant> parseBlock();

        std::expected<std::unique_ptr<VariableDeclaration>, ErrorVariant> parseVariableDeclaration( const bool locked = false );

        std::expected<std::unique_ptr<Return>, ErrorVariant> parseReturn();

        std::expected<std::unique_ptr<IfConditional>, ErrorVariant> parseIfConditional( bool just_else = false );

        std::expected<std::unique_ptr<ForLoop>, ErrorVariant> parseForLoop();

    private:
        Parser& m_parent;
        ParserUtils& m_utils;
        ErrorReporter& m_errReporter;
        TypeParser& m_typeParser;
        ParameterParser& m_paramParser;
        ExpressionParser* m_exprParser = nullptr;
};