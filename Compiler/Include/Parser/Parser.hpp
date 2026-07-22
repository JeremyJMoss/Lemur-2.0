#pragma once

/* === Dependencies ===*/

#include <expected>

/* === Imports === */

#include "Errors/Errors.hpp"
#include "AST/ParsedType.hpp"
#include "Parser/TypeParser.hpp"
#include "Parser/ExpressionParser.hpp"
#include "Parser/StatementParser.hpp"
#include "Parser/ParameterParser.hpp"
#include "Tokens/TokenStream.hpp"

/* === Forward Declarations === */

struct Statement;

class CompilationUnit;
class Token;

/* === Parser === */

class Parser 
{
    public:
        Parser(CompilationUnit& compUnit );
        
        void parse();

        std::expected<Statement*, Diagnostic> createStatement( 
            const Token& token, 
            DeclarationVisibility visibility = DeclarationVisibility::Private 
        );

    private:
        CompilationUnit& m_compUnit;
        TokenStream m_tokenStream;
        TypeParser m_typeParser;
        ParameterParser m_paramParser;
        StatementParser m_stmtParser;
        ExpressionParser m_exprParser;

        void parseNextStatement();

        std::expected<Statement*, Diagnostic> parseKeywordStatement( 
            const Token& token, 
            DeclarationVisibility visibility = DeclarationVisibility::Private 
        );

        std::expected<Statement*, Diagnostic> parseIdentifierStatement( 
            const Token& token, 
            DeclarationVisibility visibility = DeclarationVisibility::Private 
        );

        std::expected<Statement*, Diagnostic> parseExpressionStatement( const Token& token );
};
