#pragma once

/* === Dependencies === */

#include <expected>
#include <vector>
#include <string>

/* === Imports === */

#include "Errors/Errors.hpp"
#include "AST/Import.hpp"
#include "AST/ASTNode.hpp"

/* === Forward Declarations === */

struct Block;
struct ForLoop;
struct FunctionDeclaration;
struct IfConditional;
struct Return;
struct VariableDeclaration;
struct Import;

class CompilationUnit;
class ExpressionParser;
class ParameterParser;
class TypeParser;
class Parser;
class TokenStream;
class CompilerContext;

/* === StatementParser === */

class StatementParser 
{
    public:
        StatementParser( 
            Parser& parent,
            CompilationUnit& compUnit,
            TokenStream& tStream,
            CompilerContext& ctx,
            TypeParser& typeParser,
            ParameterParser& paramParser
        ) : m_parent( parent ), 
            m_compUnit( compUnit ),
            m_tokenStream( tStream ), 
            m_ctx( ctx ), 
            m_typeParser( typeParser ), 
            m_paramParser( paramParser ) {}

        void setExpressionParser( ExpressionParser* exprParser ) 
        { 
            m_exprParser = exprParser; 
        }

        std::expected<FunctionDeclaration*, Diagnostic> parseFunctionDeclaration( const bool isEntry = false, DeclarationVisibility visibility = DeclarationVisibility::Private );

        std::expected<Block*, Diagnostic> parseBlock();

        std::expected<VariableDeclaration*, Diagnostic> parseVariableDeclaration( const bool locked = false, DeclarationVisibility visibility = DeclarationVisibility::Private );

        std::expected<Return*, Diagnostic> parseReturn();

        std::expected<IfConditional*, Diagnostic> parseIfConditional( bool just_else = false );

        std::expected<ForLoop*, Diagnostic> parseForLoop();

        std::expected<ModuleDeclaration*, Diagnostic> parseModuleDeclaration();

        std::expected<Import*, Diagnostic> parseImport();

    private:
        Parser& m_parent;
        CompilationUnit& m_compUnit;
        TokenStream& m_tokenStream;
        CompilerContext& m_ctx;
        TypeParser& m_typeParser;
        ParameterParser& m_paramParser;
        ExpressionParser* m_exprParser = nullptr;

        std::expected<std::string, Diagnostic> parseModuleName();

        std::expected<std::vector<ImportedSymbol*>, Diagnostic> parseImportedSymbols();

        std::expected<ImportedSymbol*, Diagnostic> parseSymbolImport();
};