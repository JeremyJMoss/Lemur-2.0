#include <variant>
#include "Parser/Parser.hpp"
#include "AST/AllASTTypes.hpp"
#include "Errors/Errors.hpp"
#include "Utils/Logger.hpp"

/**
 * Parses all tokens and turns them into a statement list
 *
 * @return std::vector<std::unique_ptr<Statement>> List of statements
 */
void Parser::parse( std::unique_ptr<CompilationUnit>& compUnit )
{
    m_tokenStream.reset();

    m_tokenStream.initializeTokenStream(compUnit->readTokens());
    // while there are still tokens in the list continue parsing
    while ( !m_tokenStream.peek().checkTypeMatches(TokenKind::EndOfFile) ) 
    {
        if ( m_errReporter.hasFatalErrors() ) break;
        parseNextStatement(compUnit);
    }
}

void Parser::parseNextStatement( std::unique_ptr<CompilationUnit>& compUnit ) 
{
    auto current = m_tokenStream.peek();

    auto maybeStatement = createStatement( current );

    // setting up array for debugging
    const std::array attrs = {
        Attribute{ "Value", "'" + current.getValue() + "'" },
        Attribute{ "Type", "'" + toString( current.getType() ) + "'" },
        Attribute{ "Location", current.getLocation().toString() }
    };

    if ( !maybeStatement )
    {
        Logger::trace( 
            "Failed to create statement starting with token",
            attrs
        );

        std::visit(
            [&](auto&& err) {
                m_errReporter.report( std::move( err ) );
            },
            maybeStatement.error()
        );

        m_utils.recoverFromError();
        return;
    }

    Logger::trace( 
        "Successfully created statement starting with token",
        attrs
    );

    compUnit->addToAST( std::move( maybeStatement.value() ) );
}

std::expected<std::unique_ptr<Statement>, ErrorVariant> Parser::createStatement( const Token& token ) 
{

    // setting up array for debugging purposes
    const std::array attrs = {
        Attribute{ "Value", "'" + token.getValue() + "'" },
        Attribute{ "Location", token.getLocation().toString() }
    };

    if ( token.checkTypeMatches( TokenKind::Keyword ) ) 
    {
        Logger::trace( 
            "Token matched keyword statement",
            attrs
        );

        return parseKeywordStatement( token );
    }

    if ( token.checkTypeMatches( TokenKind::Identifier ) ) 
    {
        Logger::trace( 
            "Token matched identifier statement",
            attrs
        );

        return parseIdentifierStatement( token );
    }

    Logger::trace( 
        "Token matched expression statement",
        attrs
    );

    return parseExpressionStatement( token );
}

std::expected<std::unique_ptr<Statement>, ErrorVariant> Parser::parseKeywordStatement( const Token& token ) 
{
    // Constant declaration
    if ( token.checkValueMatches( TokenKeyword::Lock ) ) 
    {
        auto maybeDeclaration = m_stmtParser.parseVariableDeclaration( true );
        if ( !maybeDeclaration ) return std::unexpected( maybeDeclaration.error() );

        auto maybeEndingNode = m_utils.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
        if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

        return std::move( maybeDeclaration.value() );
    }

    // Return statement
    if ( token.checkValueMatches( TokenKeyword::Return ) ) 
    {
        auto maybeReturn = m_stmtParser.parseReturn();
        if ( !maybeReturn ) return std::unexpected( maybeReturn.error() );

        auto maybeEndingNode = m_utils.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
        if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

        return std::move( maybeReturn.value() );
    }

    // If statement
    if ( token.checkValueMatches( TokenKeyword::If ) ) return m_stmtParser.parseIfConditional();

    // For loop
    if ( token.checkValueMatches( TokenKeyword::For ) ) return m_stmtParser.parseForLoop();

    // Break statement
    if ( token.checkValueMatches( TokenKeyword::Break ) ) 
    {
        auto maybeBreak = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Break );
        if ( !maybeBreak ) return std::unexpected( maybeBreak.error() );

        auto stmt = std::make_unique<Break>();
        stmt->location = m_utils.getLocation( token );

        auto maybeEndingNode = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
        if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

        return stmt;
    }

    // Continue Statement
    if ( token.checkValueMatches( TokenKeyword::Continue ) ) 
    {
        auto maybeContinue = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Continue );
        if ( !maybeContinue ) return std::unexpected( maybeContinue.error() );

        auto stmt = std::make_unique<Continue>();
        stmt->location = m_utils.getLocation( token );

        auto maybeEndingNode = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
        if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

        return stmt;
    }

    // Function declaration
    if ( token.checkValueMatches( TokenKeyword::Fn ) ) return m_stmtParser.parseFunctionDeclaration();

    return std::unexpected( 
        CompilerError(
            "Unknown keyword: '" + token.getValue() + "'", 
            ErrorSeverity::Error,
            token.getLocation(),
            ErrorCategory::Syntax
        ) 
    );
}

std::expected<std::unique_ptr<Statement>, ErrorVariant> Parser::parseIdentifierStatement( const Token& token )
{
    // Look ahead to see if this is a declaration like: x : int = ...
    if( m_tokenStream.peek( 1 ).checkTypeMatches(TokenKind::EndOfFile)) {
        return std::unexpected(
            CompilerError(
                "Unexpected end of input",
                ErrorSeverity::Fatal,
                m_tokenStream.peek(1).getLocation(),
                ErrorCategory::Syntax
            )
        );
    };

    const Token& next = m_tokenStream.consume();

    if ( next.checkValueMatches( TokenSymbol::Colon ) ) 
    {
        auto maybeDeclaration = m_stmtParser.parseVariableDeclaration();
        if ( !maybeDeclaration ) return std::unexpected( maybeDeclaration.error() );

        auto maybeEndingNode = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
        if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

        return std::move( maybeDeclaration.value() );
    } else if (next.checkValueMatches( TokenSymbol::Assign ) ) 
    {
        auto maybeAssignment = m_exprParser.parseAssignment();
        if ( ! maybeAssignment ) return std::unexpected( maybeAssignment.error() );

        auto stmt = std::make_unique<ExpressionStatement>( std::move( maybeAssignment.value() ) );

        auto maybeEndToken = m_utils.peekBack();
        if ( !maybeEndToken ) return std::unexpected( maybeEndToken.error() );

        stmt->location = m_utils.getLocation( token, maybeEndToken.value() );

        auto maybeEndingNode = m_utils.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
        if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

        return stmt;
    }

    // Parse a postfix expression starting at the identifier.
    // This will consume the identifier and any following () chains.
    auto maybeExpression = m_exprParser.parsePostFixExpression();
    if ( !maybeExpression ) return std::unexpected( maybeExpression.error() );

    // Otherwise it's just an expression statement (covers bare identifiers too)
    auto stmt = std::make_unique<ExpressionStatement>( std::move( maybeExpression.value() ) );
    
    auto maybeEndToken = m_utils.peekBack();
    if ( !maybeEndToken ) return std::unexpected( maybeEndToken.error() );

    stmt->location = m_utils.getLocation( token, maybeEndToken.value() );

    auto maybeEndingNode = m_utils.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
    if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

    return stmt;
}

std::expected<std::unique_ptr<Statement>, ErrorVariant> Parser::parseExpressionStatement( const Token& token )
{
    // Generic Expression statement such as 1 + 12
    auto maybeExpression = m_exprParser.parseExpression();
    if ( !maybeExpression ) return std::unexpected( maybeExpression.error() );

    auto stmt = std::make_unique<ExpressionStatement>( std::move( maybeExpression.value() ) );

    auto maybeEndToken = m_utils.peekBack();
    if ( !maybeEndToken ) return std::unexpected( maybeEndToken.error() );
    
    stmt->location = m_utils.getLocation( token, maybeEndToken.value() );
    
    auto maybeEndingNode = m_utils.expect( TokenKind::Symbol, TokenSymbol::SemiColon );
    if ( !maybeEndingNode ) return std::unexpected( maybeEndingNode.error() );

    return stmt;
}
