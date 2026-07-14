#include "Parser/Parser.hpp"
#include "Driver/CompilationUnit.hpp"
#include "AST/AllASTTypes.hpp"
#include "Errors/Errors.hpp"
#include "Errors/ErrorReporter.hpp"
#include "Utils/Logger.hpp"
#include <variant>

Parser::Parser( CompilationUnit& compUnit, ErrorReporter& errReporter )
    : m_compUnit( compUnit ),
    m_errReporter( errReporter ),
    m_tokenStream( m_compUnit.readTokens() ),
    m_typeParser( m_compUnit, m_tokenStream ),
    m_paramParser( m_compUnit, m_tokenStream, m_typeParser ),
    m_stmtParser( *this, m_compUnit, m_tokenStream, m_errReporter, m_typeParser, m_paramParser ),
    m_exprParser( m_compUnit, m_tokenStream, m_typeParser, m_paramParser ) 
{
    m_stmtParser.setExpressionParser( &m_exprParser );
    m_exprParser.setStatementParser( &m_stmtParser );
}

/**
 * Parses all tokens and turns them into a statement list
 *
 * @return std::vector<std::unique_ptr<Statement>> List of statements
 */
void Parser::parse()
{
    // while there are still tokens in the list continue parsing
    while ( !m_tokenStream.peek().checkTypeMatches( TokenKind::EndOfFile ) ) 
    {
        parseNextStatement();
    }
}

void Parser::parseNextStatement() 
{
    const Token& current = m_tokenStream.peek();

    auto maybeStatement = createStatement( current );

    // setting up array for debugging
    const std::array attrs = {
        Attribute{ "Value", "'" + std::string( current.getValue() ) + "'" },
        Attribute{ "Type", std::format( "'{}'", toString( current.getType() ) ) },
        Attribute{ "Location", current.getLocation().toString() }
    };

    if ( !maybeStatement )
    {
        Logger::trace( 
            "Failed to create statement starting with token",
            attrs
        );

        m_errReporter.report( maybeStatement.error() );

        m_tokenStream.recoverFromError();
        return;
    }

    Logger::trace( 
        "Successfully created statement starting with token",
        attrs
    );

    m_compUnit.addToAST( maybeStatement.value() );
}

std::expected<Statement*, Diagnostic> Parser::createStatement( const Token& token, DeclarationVisibility visibility ) 
{
    // setting up array for debugging purposes
    const std::array attrs = {
        Attribute{ "Value", "'" + std::string( token.getValue() ) + "'" },
        Attribute{ "Location", token.getLocation().toString() }
    };

    if ( token.checkTypeMatches( TokenKind::Keyword ) ) 
    {
        Logger::trace( 
            "Token matched keyword statement",
            attrs
        );

        return parseKeywordStatement( token, visibility );
    }

    if ( token.checkTypeMatches( TokenKind::Identifier ) ) 
    {
        Logger::trace( 
            "Token matched identifier statement",
            attrs
        );

        return parseIdentifierStatement( token, visibility );
    }

    Logger::trace( 
        "Token matched expression statement",
        attrs
    );

    if (DeclarationVisibility::Public == visibility)
    {
        return std::unexpected(
            Diagnostic(
                "'export' cannot be applied to expression",
                ErrorCategory::Syntax,
                ErrorSeverity::Error,
                token.getLocation()
            )
        );
    }

    return parseExpressionStatement( token );
}

std::expected<Statement*, Diagnostic> Parser::parseKeywordStatement( const Token& token, DeclarationVisibility visibility ) 
{
    // Constant declaration
    if ( token.checkValueMatches( TokenKeyword::Lock ) ) 
    {
        auto maybeDeclaration = m_stmtParser.parseVariableDeclaration( true, visibility );
        if ( !maybeDeclaration ) return std::unexpected( maybeDeclaration.error() );

        auto maybeEndingNode = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
        if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

        return maybeDeclaration.value();
    }

    // Function declaration
    if ( token.checkValueMatches( TokenKeyword::Fn ) ) {
        return m_stmtParser.parseFunctionDeclaration( false, visibility );
    }

    // Entry Function Declaration
    if ( token.checkValueMatches( TokenKeyword::Entry ) ) {
        if (DeclarationVisibility::Public == visibility)
        {
            return std::unexpected(
                Diagnostic(
                    "'export' cannot be applied to entry point function",
                    ErrorCategory::Syntax,
                    ErrorSeverity::Error,
                    token.getLocation()
                )
            );
        }

        return m_stmtParser.parseFunctionDeclaration( true );
    }

    // Return statement
    if ( token.checkValueMatches( TokenKeyword::Return ) ) 
    {
        if (DeclarationVisibility::Public == visibility)
        {
            return std::unexpected(
                Diagnostic(
                    "'export' cannot be applied to return statement",
                    ErrorCategory::Syntax,
                    ErrorSeverity::Error,
                    token.getLocation()
                )
            );
        }

        auto maybeReturn = m_stmtParser.parseReturn();
        if ( !maybeReturn ) return std::unexpected( maybeReturn.error() );

        auto maybeEndingNode = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
        if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

        return maybeReturn.value();
    }

    // If statement
    if ( token.checkValueMatches( TokenKeyword::If ) ) {
        if (DeclarationVisibility::Public == visibility)
        {
            return std::unexpected(
                Diagnostic(
                    "'export' cannot be applied to if statement",
                    ErrorCategory::Syntax,
                    ErrorSeverity::Error,
                    token.getLocation()
                )
            );
        }

        return m_stmtParser.parseIfConditional();
    }

    // For loop
    if ( token.checkValueMatches( TokenKeyword::For ) ) {
        if (DeclarationVisibility::Public == visibility)
        {
            return std::unexpected(
                Diagnostic(
                    "'export' cannot be applied to for loop",
                    ErrorCategory::Syntax,
                    ErrorSeverity::Error,
                    token.getLocation()
                )
            );
        }

        return m_stmtParser.parseForLoop();
    }

    // Break statement
    if ( token.checkValueMatches( TokenKeyword::Break ) ) 
    {
        if (DeclarationVisibility::Public == visibility)
        {
            return std::unexpected(
                Diagnostic(
                    "'export' cannot be applied to break statement",
                    ErrorCategory::Syntax,
                    ErrorSeverity::Error,
                    token.getLocation()
                )
            );
        }

        auto maybeBreak = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Break );
        if ( !maybeBreak ) return std::unexpected( maybeBreak.error() );

        auto stmt = m_compUnit.allocate<Break>();
        stmt->location = SourceRange::getLocation( token );

        auto maybeEndingNode = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
        if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

        return stmt;
    }

    // Continue Statement
    if ( token.checkValueMatches( TokenKeyword::Continue ) ) 
    {
        if (DeclarationVisibility::Public == visibility)
        {
            return std::unexpected(
                Diagnostic(
                    "'export' cannot be applied to continue statement",
                    ErrorCategory::Syntax,
                    ErrorSeverity::Error,
                    token.getLocation()
                )
            );
        }

        auto maybeContinue = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Continue );
        if ( !maybeContinue ) return std::unexpected( maybeContinue.error() );

        auto stmt = m_compUnit.allocate<Continue>();
        stmt->location = SourceRange::getLocation( token );

        auto maybeEndingNode = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
        if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

        return stmt;
    }

    if ( token.checkValueMatches( TokenKeyword::Export ) ) 
    {
        if ( DeclarationVisibility::Public == visibility )
        {
            return std::unexpected(
                Diagnostic(
                    "'export' cannot be applied more than once",
                    ErrorCategory::Syntax,
                    ErrorSeverity::Error,
                    token.getLocation()
                )
            );
        }

        auto maybeExport = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Export );
        
        if ( !maybeExport ) 
        {
            return std::unexpected( maybeExport.error() );
        }

        const Token& current = m_tokenStream.peek();

        auto stmt = createStatement(current, DeclarationVisibility::Public);

        if (!stmt) return std::unexpected( stmt.error() );

        return stmt;
    }

    if ( token.checkValueMatches( TokenKeyword::Import ) ) {
        if (DeclarationVisibility::Public == visibility)
        {
            return std::unexpected(
                Diagnostic(
                    "'export' cannot be applied to import header directive",
                    ErrorCategory::Syntax,
                    ErrorSeverity::Error,
                    token.getLocation()
                )
            );
        }

        auto maybeImportStatement = m_stmtParser.parseImport();
        if ( !maybeImportStatement ) return std::unexpected( maybeImportStatement.error() );

        auto maybeEndingNode = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
        if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

        return maybeImportStatement.value();
    }

    if ( token.checkValueMatches( TokenKeyword::Module ) ) {
        if (DeclarationVisibility::Public == visibility)
        {
            return std::unexpected(
                Diagnostic(
                    "'export' cannot be applied to module header directive",
                    ErrorCategory::Syntax,
                    ErrorSeverity::Error,
                    token.getLocation()
                )
            );
        }

        auto maybeDeclaration = m_stmtParser.parseModuleDeclaration();
        if ( !maybeDeclaration ) return std::unexpected( maybeDeclaration.error() );

        auto maybeEndingNode = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
        if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

        return maybeDeclaration.value();
    }

    return std::unexpected( 
        Diagnostic(
            std::format(
                "Unknown keyword: '{}'",
                token.getValue()
            ),
            ErrorCategory::Syntax,
            ErrorSeverity::Error,
            token.getLocation()
        ) 
    );
}

std::expected<Statement*, Diagnostic> Parser::parseIdentifierStatement( const Token& token, DeclarationVisibility visibility )
{
    const Token& next = m_tokenStream.peek(1);
    
    // Look ahead to see if this is a declaration like: x : int = ...
    if( next.checkTypeMatches(TokenKind::EndOfFile)) {
        return std::unexpected(
            UnexpectedEndOfInputDiagnostic(
                next.getLocation()
            )
        );
    };

    if ( next.checkValueMatches( TokenSymbol::Colon ) ) 
    {
        auto maybeDeclaration = m_stmtParser.parseVariableDeclaration( false, visibility );
        if ( !maybeDeclaration ) return std::unexpected( maybeDeclaration.error() );

        auto maybeEndingNode = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
        if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

        return maybeDeclaration.value();
    } else if (next.checkValueMatches( TokenSymbol::Assign ) ) 
    {
        if ( DeclarationVisibility::Public == visibility )
        {
            return std::unexpected(
                Diagnostic(
                    "'export' cannot be applied to assignment statement",
                    ErrorCategory::Syntax,
                    ErrorSeverity::Error,
                    token.getLocation()
                )
            );
        }

        auto maybeAssignment = m_exprParser.parseAssignment();
        if ( ! maybeAssignment ) return std::unexpected( maybeAssignment.error() );

        auto stmt = m_compUnit.allocate<ExpressionStatement>( maybeAssignment.value() );

        auto maybeEndingNode = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
        if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

        stmt->location = SourceRange::getLocation( token, maybeEndingNode.value() );

        return stmt;
    }

    if (DeclarationVisibility::Public == visibility)
    {
        return std::unexpected(
            Diagnostic(
                "'export' cannot be applied to expression",
                ErrorCategory::Syntax,
                ErrorSeverity::Error,
                token.getLocation()
            )
        );
    }

    // Parse a postfix expression starting at the identifier.
    // This will consume the identifier and any following () chains.
    auto maybeExpression = m_exprParser.parsePostFixExpression();
    if ( !maybeExpression ) return std::unexpected( maybeExpression.error() );

    // Otherwise it's just an expression statement (covers bare identifiers too)
    auto stmt = m_compUnit.allocate<ExpressionStatement>( maybeExpression.value() );

    auto maybeEndingNode = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
    if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

    stmt->location = SourceRange::getLocation( token, maybeEndingNode.value() );

    return stmt;
}

std::expected<Statement*, Diagnostic> Parser::parseExpressionStatement( const Token& token )
{
    // Generic Expression statement such as 1 + 12
    auto maybeExpression = m_exprParser.parseExpression();
    if ( !maybeExpression ) return std::unexpected( maybeExpression.error() );

    auto stmt = m_compUnit.allocate<ExpressionStatement>( maybeExpression.value() );
        
    auto maybeEndingNode = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
    if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

    stmt->location = SourceRange::getLocation( token, maybeEndingNode.value() );

    return stmt;
}
