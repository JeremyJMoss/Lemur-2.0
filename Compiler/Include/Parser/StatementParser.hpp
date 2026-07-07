#pragma once

#include <expected>
#include "Errors/Errors.hpp"

/* === Forward Declarations === */

struct Block;
struct ForLoop;
struct FunctionDeclaration;
struct IfConditional;
struct Return;
struct VariableDeclaration;

class CompilationUnit;
class ExpressionParser;
class ParameterParser;
class TypeParser;
class Parser;
class TokenStream;
class ErrorReporter;

/* === StatementParser === */

class StatementParser 
{
    public:
        StatementParser( 
            Parser& parent,
            CompilationUnit& compUnit,
            TokenStream& tStream,
            ErrorReporter& errReporter,
            TypeParser& typeParser,
            ParameterParser& paramParser
        ) : m_parent( parent ), 
            m_compUnit( compUnit ),
            m_tokenStream( tStream ), 
            m_errReporter( errReporter ), 
            m_typeParser( typeParser ), 
            m_paramParser( paramParser ) {}

        void setExpressionParser( ExpressionParser* exprParser ) 
        { 
            m_exprParser = exprParser; 
        }

        std::expected<FunctionDeclaration*, ErrorVariant> parseFunctionDeclaration( const bool isEntry = false );

        std::expected<Block*, ErrorVariant> parseBlock();

        std::expected<VariableDeclaration*, ErrorVariant> parseVariableDeclaration( const bool locked = false );

        std::expected<Return*, ErrorVariant> parseReturn();

        std::expected<IfConditional*, ErrorVariant> parseIfConditional( bool just_else = false );

        std::expected<ForLoop*, ErrorVariant> parseForLoop();

        std::expected<ModuleDeclaration*, ErrorVariant> parseModuleDeclaration();

    private:
        Parser& m_parent;
        CompilationUnit& m_compUnit;
        TokenStream& m_tokenStream;
        ErrorReporter& m_errReporter;
        TypeParser& m_typeParser;
        ParameterParser& m_paramParser;
        ExpressionParser* m_exprParser = nullptr;
};