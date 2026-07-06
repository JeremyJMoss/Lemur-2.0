#pragma once

/* === Imports === */

#include <iostream>
#include <expected>
#include "Tokens/Token.hpp"
#include "AST/AllASTTypes.hpp"
#include "AST/ParsedType.hpp"
#include "Parser/TypeParser.hpp"
#include "Parser/ExpressionParser.hpp"
#include "Parser/StatementParser.hpp"
#include "Parser/ParameterParser.hpp"
#include "Tokens/TokenStream.hpp"
#include "Driver/CompilationUnit.hpp"

/* === Forward Declarations === */
class ErrorReporter;

/* === Parser === */

using fileId = std::size_t;

class Parser 
{
    public:
        Parser( CompilationUnit& compUnit, ErrorReporter& errReporter )
            : m_compUnit( compUnit ),
            m_errReporter( errReporter ),
            m_tokenStream( m_compUnit.readTokens() ),
            m_typeParser( m_compUnit, m_tokenStream ),
            m_paramParser( m_compUnit, m_tokenStream, m_typeParser ),
            m_stmtParser( *this, m_compUnit, m_tokenStream, m_errReporter, m_typeParser, m_paramParser ),
            m_exprParser( m_compUnit, m_tokenStream, m_typeParser, m_paramParser ) {
                m_stmtParser.setExpressionParser( &m_exprParser );
                m_exprParser.setStatementParser( &m_stmtParser );
            }
        
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
