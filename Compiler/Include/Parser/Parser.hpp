#pragma once

/* === Imports === */

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <regex>
#include <unordered_map>
#include <algorithm>
#include <expected>
#include "Tokens/Token.hpp"
#include "AST/AllASTTypes.hpp"
#include "AST/ParsedType.hpp"
#include "Errors/ErrorReporter.hpp"
#include "Parser/ParserUtils.hpp"
#include "Parser/TypeParser.hpp"
#include "Parser/ExpressionParser.hpp"
#include "Parser/StatementParser.hpp"

/* === Parser === */

class Parser 
{
    public:
        Parser( ErrorReporter& errReporter ) 
            : m_errReporter( errReporter ),
            m_typeParser( m_utils ),
            m_paramParser( m_utils, m_typeParser ),
            m_stmtParser( *this, m_utils, m_errReporter, m_typeParser, m_paramParser ),
            m_exprParser( m_utils, m_typeParser, m_paramParser ){
                m_stmtParser.setExpressionParser( &m_exprParser );
                m_exprParser.setStatementParser( &m_stmtParser );
            }
        
        std::unique_ptr<AST> parse( std::vector<Token>&& inputTokens );

        std::expected<std::unique_ptr<Statement>, ErrorVariant> createStatement( const Token& token );

    private:
        ErrorReporter& m_errReporter;
        std::vector<std::unique_ptr<Statement>> m_statements;
        ParserUtils m_utils;
        TypeParser m_typeParser;
        ParameterParser m_paramParser;
        StatementParser m_stmtParser;
        ExpressionParser m_exprParser;

        void resetState( std::vector<Token>&& inputTokens );

        void parseNextStatement();

        std::expected<std::unique_ptr<Statement>, ErrorVariant> parseKeywordStatement( const Token& token );

        std::expected<std::unique_ptr<Statement>, ErrorVariant> parseIdentifierStatement( const Token& token );

        std::expected<std::unique_ptr<Statement>, ErrorVariant> parseExpressionStatement( const Token& token );
};
