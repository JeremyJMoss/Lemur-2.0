#include "Parser/StatementParser.hpp"
#include "Parser/Parser.hpp"
#include "Utils/Logger.hpp"
#include "AST/ASTPrinter.hpp"

std::expected<FunctionDeclaration*, ErrorVariant> StatementParser::parseFunctionDeclaration() 
{
    Logger::debug( 
        "Parsing function declaration"
    );

    const Token& front = m_tokenStream.peek();

    if (front.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected(UnexpectedEndOfInputError(front.getLocation()));
    }

    auto maybeFunctionKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Fn );
    if ( !maybeFunctionKeyword ) 
    {
        std::visit( [&] ( auto&& err ) 
            {
                m_errReporter.report( std::move( err ) );
            }, 
            maybeFunctionKeyword.error()
        );
    }

    auto peekedToken = m_tokenStream.peek();

    if (!peekedToken.checkTypeMatches(TokenKind::Identifier)) {
        return std::unexpected(UnexpectedTypeError( TokenKind::Identifier, peekedToken.getType(), peekedToken.getLocation()));
    }

    auto idToken = m_tokenStream.consume();

    Logger::trace(
        "Function name parsed", 
        std::to_array<Attribute>({
            { "Identifier", "'" + idToken.getValue() + "'" }
        })
    );

    auto identifier = m_compUnit.allocate<Identifier>( idToken.getValue() );

    identifier->location = SourceRange::getLocation( idToken );

    auto maybeParameters = m_paramParser.parseFunctionParameters();
    if ( !maybeParameters ) return std::unexpected( maybeParameters.error() );
    auto parameters = std::move( maybeParameters.value() );

    Logger::trace(
        "Parameters parsed", 
        std::to_array<Attribute>({
            { "Count", std::to_string( parameters.size() ) }
        })
    );

    auto maybeColon = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Colon );
    if ( !maybeColon ) return std::unexpected( maybeColon.error() );

    auto maybeReturnType = m_typeParser.parseType();
    if ( !maybeReturnType ) return std::unexpected( maybeReturnType.error() );

    Logger::trace(
        "Return type parsed", 
        std::to_array<Attribute>({
            { "Type", "'" + ASTPrinter::getParsedType( maybeReturnType.value()->kind ) + "'" }
        })
    );

    auto maybeBody = parseBlock();
    if ( !maybeBody ) return std::unexpected( maybeBody.error() );
    auto block = maybeBody.value();

    Logger::trace(
        "Function body parsed"
    );

    auto funDec = m_compUnit.allocate<FunctionDeclaration>(
        identifier, 
        maybeReturnType.value(), 
        parameters, 
        block,
        true
    );

    funDec->location = { front.getLocation().start, funDec->body->location.end, front.getLocation().fileId};

    Logger::debug(
        "Function declaration parsed successfully",
        std::to_array<Attribute>({
            { "Name", "'" + idToken.getValue() + "'" }
        })
    );

    return funDec;
}

std::expected<Block*, ErrorVariant> StatementParser::parseBlock() 
{
    const Token& front = m_tokenStream.peek();

    if (front.checkTypeMatches( TokenKind::EndOfFile )){
        return std::unexpected(UnexpectedEndOfInputError(front.getLocation()));
    }

    Logger::debug(
        "Parsing block", 
        std::to_array<Attribute>({ 
            { "StartToken", front.getLocation().toString() } 
        })
    );

    auto maybeFrontBrace = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::LBrace );
    if ( !maybeFrontBrace ) return std::unexpected( maybeFrontBrace.error() );

    std::vector<Statement*> body;

    Logger::trace( "Parsing statements inside block" );

    auto current = m_tokenStream.peek();

    while ( true ) 
    {
        if (current.checkTypeMatches( TokenKind::EndOfFile ) ) {
            return std::unexpected(UnexpectedEndOfInputError(current.getLocation()));
        }

        if ( current.checkMatches( TokenKind::Symbol, TokenSymbol::RBrace ) ) 
        {
            auto maybeClosingBrace = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::RBrace );
            if ( !maybeClosingBrace ) return std::unexpected( maybeClosingBrace.error() );

            Logger::trace(
                "Block closed", 
                std::to_array<Attribute>({ 
                    { "EndToken", maybeClosingBrace.value().get().getLocation().toString() } 
                })
            );

            break;
        }

        auto maybeStatement = m_parent.createStatement( current );
        if ( !maybeStatement ) 
        {
            std::visit( [&] ( auto&& err ) 
            {
                m_errReporter.report( std::move( err ) );
            }, maybeStatement.error() );

            Logger::trace( "Recovering from error inside block" );

            m_tokenStream.recoverFromError();

            current = m_tokenStream.peek();
            continue;
        }

        body.emplace_back( maybeStatement.value() );

        current = m_tokenStream.peek();
    }

    size_t bodySize = body.size();

    auto block = m_compUnit.allocate<Block>( body );

    block->location = SourceRange::getLocation( front, current );

    Logger::debug( 
        "Parsing statements inside block successful",
        std::to_array<Attribute>({
            { "StatementCount", std::to_string( bodySize ) }
        })
    );

    return block;
}

std::expected<VariableDeclaration*, ErrorVariant> StatementParser::parseVariableDeclaration( const bool locked ) 
{
    Logger::debug(
        "Parsing variable declaration", 
        std::to_array<Attribute>({ 
            { "Locked", ( locked ? "true" : "false" ) } 
        })
    );

    const Token& front = m_tokenStream.peek();

    if (front.checkTypeMatches( TokenKind::EndOfFile)) {
        return std::unexpected( UnexpectedEndOfInputError( front.getLocation() ) );
    }

    if ( locked ) 
    {
        auto maybeLocked = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Lock );
        if ( !maybeLocked )
        {
            // Report locked variable missing lock keyword
            m_errReporter.report(
                CompilerError(
                    "Locked variable missing lock keyword",
                    ErrorSeverity::Warning,
                    front.getLocation(),
                    ErrorCategory::Syntax
                )
            );
        }
    }

    const Token& peekedToken = m_tokenStream.peek();

    if (!peekedToken.checkTypeMatches(TokenKind::Identifier)) {
        return std::unexpected(UnexpectedTypeError( TokenKind::Identifier, peekedToken.getType(), peekedToken.getLocation()));
    }

    const Token& idToken = m_tokenStream.consume();

    Logger::trace(
        "Consumed variable identifier", 
        std::to_array<Attribute>({ 
            { "Identifier", "'" + idToken.getValue() + "'" } 
        })
    );

    auto identifier = m_compUnit.allocate<Identifier>( idToken.getValue() );
    identifier->location = SourceRange::getLocation( front, idToken );

    auto maybeColon = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Colon );
    if ( !maybeColon ) return std::unexpected( maybeColon.error() );

    auto maybeParsedType = m_typeParser.parseType();
    if ( !maybeParsedType )
    {
        return std::unexpected(
            CompilerError(
                "Could not parse type.",
                ErrorSeverity::Error,
                maybeColon.value().get().getLocation(),
                ErrorCategory::Syntax
            )
        );
    }

    auto varType = maybeParsedType.value();

    Logger::trace(
        "Parsed variable type", 
        std::to_array<Attribute>({ 
            { "Type", "'" + ASTPrinter::getParsedType( varType->kind ) + "'" } 
        })
    );

    const Token& current = m_tokenStream.peek();

    if (current.checkTypeMatches(TokenKind::EndOfFile)) {
        return std::unexpected(UnexpectedEndOfInputError( current.getLocation() ));
    }

    Expression* initialiser = nullptr;

    if ( current.checkMatches( TokenKind::Symbol, TokenSymbol::Assign ) )
    {
        Logger::trace(
            "Detected assignment in variable declaration", 
            std::to_array<Attribute>({ 
                { "Identifier", "'" + idToken.getValue() + "'" } 
            })
        );

        auto maybeInitialiser = m_exprParser->parseInitialiser();

        if ( !maybeInitialiser ) 
        {
            return std::unexpected( maybeInitialiser.error() );
        }
        else 
        {
            initialiser = maybeInitialiser.value();
        }
    }

    auto endLocation = initialiser != nullptr ? initialiser->location.end : varType->location.end;

    auto decl = m_compUnit.allocate<VariableDeclaration>( 
        identifier, 
        varType, 
        locked, 
        initialiser 
    );

    decl->location = {front.getLocation().start, endLocation, front.getLocation().fileId };

    Logger::debug(
        "Completed variable declaration", 
        std::to_array<Attribute>({ 
            { "Identifier", "'" + decl->identifier->name + "'" },
            { "HasInitialiser", ( initialiser ? "true" : "false" ) }
        })
    );

    return decl;
}

std::expected<Return*, ErrorVariant> StatementParser::parseReturn() 
{
    Logger::debug( "Parsing return statement" );

    const Token& front = m_tokenStream.peek();

    if ( front.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( front.getLocation() ) );
    }

    auto maybeReturnKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Return );
    if ( !maybeReturnKeyword ) return std::unexpected( maybeReturnKeyword.error() );

    const Token& next = m_tokenStream.peek();

    if ( next.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( next.getLocation() ) );
    }

    Expression* value = nullptr;

    Logger::trace( "Checking for return value expression" );

    // bare  `return ;`
    if ( !next.checkMatches( TokenKind::Symbol, TokenSymbol::SemiColon ) ) {
        auto maybeValue = m_exprParser->parseExpression();
        if ( !maybeValue ) 
        {
            return std::unexpected( 
                CompilerError(
                    "Unable to parse return statement. Malformed expression statement after return keyword",
                    ErrorSeverity::Error,
                    next.getLocation(),
                    ErrorCategory::Syntax
                )
            );
        }
        value = maybeValue.value();
    }

    bool hasReturn = value != nullptr;

    Logger::trace( "Parsed return value expression" );

    auto ret = m_compUnit.allocate<Return>( value );

    auto endLocation = hasReturn ? ret->value->location.end : front.getLocation().end;

    ret->location = { front.getLocation().start, endLocation, front.getLocation().fileId };

    Logger::debug( 
        "Constructed return statement", 
        std::to_array<Attribute>({ 
            { "HasValue", ( hasReturn ? "true" : "false" ) } 
        })
    );

    return ret;
}

std::expected<IfConditional*, ErrorVariant> StatementParser::parseIfConditional( bool justElse ) 
{
    std::string stmtType = justElse ? "else" : "if";
    Logger::debug( "Parsing " + stmtType + " statement" );

    const Token& front = m_tokenStream.peek();

    if ( !front.checkTypeMatches( TokenKind::Keyword )) {
        return std::unexpected( UnexpectedTypeError( TokenKind::Keyword, front.getType(), front.getLocation() ) );
    }

    m_tokenStream.consume();

    Expression* condition = nullptr;

    // else case does not need condition
    if ( !justElse ) 
    {
        Logger::trace( "Parsing condition for if statement" );

        bool hasFrontParenthesis = false;
        // Except optional parenthesis around if statement condition
        auto maybeFrontParenthesis = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::LParens );
        if ( maybeFrontParenthesis ) hasFrontParenthesis = true;

        auto maybeCondition = m_exprParser->parseExpression();
        if ( !maybeCondition ) 
        {
            return std::unexpected( 
                maybeCondition.error()
            );
        }

        condition = maybeCondition.value();

        if ( hasFrontParenthesis )
        {
            Logger::debug( "Detected optional parentheses around conditional expression" );

            auto maybeClosingParenthesis = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::RParens );
            if ( !maybeClosingParenthesis )
            {
                return std::unexpected(
                    maybeClosingParenthesis.error()
                );
            }
        }
        Logger::trace( "Parsed condition for if statement" );
    }

    Logger::trace( "Parsing body for " + stmtType + " statement" );

    auto maybeBody = parseBlock();
    if ( !maybeBody )
    {
        return std::unexpected(
            maybeBody.error()
        );
    }

    IfConditional* elseStatement = nullptr;

    auto next = m_tokenStream.peek();

    if ( next.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( next.getLocation() ) );
    }
    
    if ( next.checkMatches( TokenKind::Keyword, TokenKeyword::Else ) ) 
    {
        Logger::trace( "Detected 'else', checking for 'if' to parse else-if or else body" );

        next = m_tokenStream.peek( 1 );

        if ( next.checkTypeMatches( TokenKind::EndOfFile )) {
            return std::unexpected( UnexpectedEndOfInputError( next.getLocation() ) );
        }

        if ( next.checkMatches( TokenKind::Keyword, TokenKeyword::If ) ) 
        {
            Logger::trace( "Detected else-if branch, calling parseIfConditional recursively" );

            auto maybeIf = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::If );
            if ( !maybeIf ) return std::unexpected( maybeIf.error() );

            auto maybeElseStatement = parseIfConditional();
            if ( !maybeElseStatement ) return std::unexpected( maybeElseStatement.error() );
            elseStatement = maybeElseStatement.value();
        } 
        else {
            Logger::trace( "Detected 'else' branch, parsing recursively" );

            auto maybeElseStatement = parseIfConditional( true );
            if ( !maybeElseStatement ) return std::unexpected( maybeElseStatement.error() );
            elseStatement = maybeElseStatement.value();
        }
    }

    auto endLocation = elseStatement == nullptr ? maybeBody.value()->location.end : elseStatement->location.end;

    auto ifStmt = m_compUnit.allocate<IfConditional>( 
        condition, 
        elseStatement, 
        maybeBody.value() 
    );

    ifStmt->location = { front.getLocation().start, endLocation, front.getLocation().fileId };

    Logger::debug( "Successfully parsed " + stmtType + " statement" );

    return ifStmt;
}

std::expected<ForLoop*, ErrorVariant> StatementParser::parseForLoop() 
{
    Logger::debug( "Parsing for loop statement" );

    auto maybeForKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::For );
    if ( !maybeForKeyword ) return std::unexpected( maybeForKeyword.error() );

    auto maybeFrontParens = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::LParens );
    if ( !maybeFrontParens ) return std::unexpected( maybeFrontParens.error() );

    Logger::trace( "Parsing loop variable Declaration" );

    auto maybeLoopVar = parseVariableDeclaration();
    if ( !maybeLoopVar ) return std::unexpected( maybeLoopVar.error() );

    auto maybeInKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::In );
    if ( !maybeInKeyword ) return std::unexpected( maybeInKeyword.error() );

    Logger::trace( "Parsing iterable expression for loop" );

    auto maybeIterable = m_exprParser->parseExpression();
    if ( !maybeIterable )
    {
        return std::unexpected(
            maybeIterable.error()
        );
    } 

    Expression* step = nullptr;
    Expression* where = nullptr;

    const Token& current = m_tokenStream.peek();

    if ( current.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( current.getLocation() ) );
    }

    if ( current.checkMatches( TokenKind::Keyword, TokenKeyword::Step ) ) 
    {
        Logger::trace( "Parsing 'step' expression in for loop" );

        auto maybeStepKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Step );
        if ( !maybeStepKeyword ) return std::unexpected( maybeStepKeyword.error() );

        auto maybeStep = m_exprParser->parseExpression();
        if ( !maybeStep )
        {
            m_errReporter.report(
                CompilerError(
                    "Malformed or missing step statement inside loop parameters",
                    ErrorSeverity::Error,
                    maybeStepKeyword.value().get().getLocation(),
                    ErrorCategory::Syntax
                )
            );
        }
        else
        {
            step = maybeStep.value();
        }

        const Token& next = m_tokenStream.peek();

        if ( next.checkTypeMatches( TokenKind::EndOfFile )) {
            return std::unexpected( UnexpectedEndOfInputError( next.getLocation() ) );
        }

        if ( next.checkMatches( TokenKind::Keyword, TokenKeyword::Where ) ) 
        {
            Logger::trace( "Parsing optional 'where' clause in for loop" );

            auto maybeWhereKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Where );
            if ( !maybeWhereKeyword ) return std::unexpected( maybeWhereKeyword.error() );

            auto maybeWhere = m_exprParser->parseExpression();
            if ( !maybeWhere )
            {
                m_errReporter.report(
                    CompilerError(
                        "Malformed or missing where statement inside loop parameters",
                        ErrorSeverity::Error,
                        maybeWhereKeyword.value().get().getLocation(),
                        ErrorCategory::Syntax
                    )
                );
            }
            else
            {
                where = maybeWhere.value();
            }
        }
    } 
    else if ( current.checkMatches( TokenKind::Keyword, TokenKeyword::Where ) ) 
    {
        Logger::trace( "Parsing 'where' clause in for loop without step" );

        auto maybeWhereKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Where );
        if ( !maybeWhereKeyword ) return std::unexpected( maybeWhereKeyword.error() );

        auto maybeWhere = m_exprParser->parseExpression();
        if ( !maybeWhere )
        {
            m_errReporter.report(
                CompilerError(
                    "Malformed or missing where statement inside loop parameters",
                    ErrorSeverity::Error,
                    maybeWhereKeyword.value().get().getLocation(),
                    ErrorCategory::Syntax
                )
            );
        }
        else
        {
            where = maybeWhere.value();
        }
    }

    auto maybeClosingParens = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::RParens );
    if ( !maybeClosingParens ) return std::unexpected( maybeClosingParens.error() );

    Logger::trace( "Parsing for loop body block" );
    
    auto maybeBody = parseBlock();
    if ( !maybeBody )
    {
        return std::unexpected(
            maybeBody.error()
        );
    }

    auto forLoop = m_compUnit.allocate<ForLoop>( 
        maybeLoopVar.value(), 
        maybeIterable.value(), 
        step, 
        where, 
        maybeBody.value() 
    );

    forLoop->location = { maybeForKeyword.value().get().getLocation().start, forLoop->body->location.end, forLoop->body->location.fileId };

    Logger::debug( "Successfully parsed for loop statement" );

    return forLoop;
}