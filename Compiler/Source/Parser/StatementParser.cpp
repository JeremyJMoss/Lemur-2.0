#include "Parser/StatementParser.hpp"
#include "Parser/Parser.hpp"
#include "Utils/Logger.hpp"

std::expected<std::unique_ptr<FunctionDeclaration>, ErrorVariant> StatementParser::parseFunctionDeclaration() 
{
    Logger::debug( 
        "Parsing function declaration"
    );

    auto front = m_tokenStream.peek();

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

    auto identifier = std::make_unique<Identifier>( idToken.getValue() );

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
            { "Type", "'" + toString( maybeReturnType.value()->kind ) + "'" }
        })
    );

    auto maybeBody = parseBlock();
    if ( !maybeBody ) return std::unexpected( maybeBody.error() );
    auto block = std::move( maybeBody.value() );

    Logger::trace(
        "Function body parsed"
    );

    auto funDec = std::make_unique<FunctionDeclaration>(
        std::move( identifier ), 
        std::move( maybeReturnType.value() ), 
        std::move( parameters ), 
        std::move( block ),
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

std::expected<std::unique_ptr<Block>, ErrorVariant> StatementParser::parseBlock() 
{
    auto front = m_tokenStream.peek();

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

    std::vector<std::unique_ptr<Statement>> body;

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

        body.emplace_back( std::move( maybeStatement.value() ) );

        current = m_tokenStream.peek();
    }

    size_t bodySize = body.size();

    auto block = std::make_unique<Block>( std::move( body ) );

    block->location = SourceRange::getLocation( front, current );

    Logger::debug( 
        "Parsing statements inside block successful",
        std::to_array<Attribute>({
            { "StatementCount", std::to_string( bodySize ) }
        })
    );

    return block;
}

std::expected<std::unique_ptr<VariableDeclaration>, ErrorVariant> StatementParser::parseVariableDeclaration( const bool locked ) 
{
    Logger::debug(
        "Parsing variable declaration", 
        std::to_array<Attribute>({ 
            { "Locked", ( locked ? "true" : "false" ) } 
        })
    );

    auto frontToken = m_tokenStream.peek();

    if (frontToken.checkTypeMatches( TokenKind::EndOfFile)) {
        return std::unexpected( UnexpectedEndOfInputError( frontToken.getLocation() ) );
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
                    frontToken.getLocation(),
                    ErrorCategory::Syntax
                )
            );
        }
    }

    auto peekedToken = m_tokenStream.peek();

    if (!peekedToken.checkTypeMatches(TokenKind::Identifier)) {
        return std::unexpected(UnexpectedTypeError( TokenKind::Identifier, peekedToken.getType(), peekedToken.getLocation()));
    }

    auto idToken = m_tokenStream.consume();

    Logger::trace(
        "Consumed variable identifier", 
        std::to_array<Attribute>({ 
            { "Identifier", "'" + idToken.getValue() + "'" } 
        })
    );

    auto identifier = std::make_unique<Identifier>( idToken.getValue() );
    identifier->location = SourceRange::getLocation( frontToken, idToken );

    auto maybeColon = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Colon );
    if ( !maybeColon ) return std::unexpected( maybeColon.error() );

    std::unique_ptr<ParsedType> varType;
    auto maybeParsedType = m_typeParser.parseType();
    if ( !maybeParsedType )
    {
        Logger::debug(
            "Type parsing failed, using inferred type", 
            std::to_array<Attribute>({ 
                { "Identifier", "'" + idToken.getValue() + "'" } 
            })
        );

        varType = std::make_unique<ParsedInferredType>();
        m_errReporter.report(
            CompilerError(
                "Could not parse type. Converted to inferred type.",
                ErrorSeverity::Warning,
                maybeColon.value().get().getLocation(),
                ErrorCategory::Syntax
            )
        );
    }
    else
    {
        varType = std::move( maybeParsedType.value() );

        Logger::trace(
            "Parsed variable type", 
            std::to_array<Attribute>({ 
                { "Type", "'" + toString( varType->kind ) + "'" } 
            })
        );
    }

    auto current = m_tokenStream.peek();

    if (current.checkTypeMatches(TokenKind::EndOfFile)) {
        return std::unexpected(UnexpectedEndOfInputError( current.getLocation() ));
    }

    std::unique_ptr<Expression> initialiser;

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
            initialiser = std::move( maybeInitialiser.value() );
        }
    }

    auto endLocation = initialiser != nullptr ? initialiser->location.end : varType->location.end;

    auto decl = std::make_unique<VariableDeclaration>( 
        std::move( identifier ), 
        std::move( varType ), 
        locked, 
        std::move( initialiser ) 
    );

    decl->location = {frontToken.getLocation().start, endLocation, frontToken.getLocation().fileId };

    Logger::debug(
        "Completed variable declaration", 
        std::to_array<Attribute>({ 
            { "Identifier", "'" + decl->identifier->name + "'" },
            { "HasInitialiser", ( initialiser ? "true" : "false" ) }
        })
    );

    return decl;
}

std::expected<std::unique_ptr<Return>, ErrorVariant> StatementParser::parseReturn() 
{
    Logger::debug( "Parsing return statement" );

    auto front = m_tokenStream.peek();

    if ( front.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( front.getLocation() ) );
    }

    auto maybeReturnKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Return );
    if ( !maybeReturnKeyword ) return std::unexpected( maybeReturnKeyword.error() );

    auto next = m_tokenStream.peek();

    if ( next.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( next.getLocation() ) );
    }

    std::unique_ptr<Expression> value;

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
        value = std::move( maybeValue.value() );
    }

    bool hasReturn = value != nullptr;

    Logger::trace( "Parsed return value expression" );

    auto ret = std::make_unique<Return>( std::move( value ) );

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

std::expected<std::unique_ptr<IfConditional>, ErrorVariant> StatementParser::parseIfConditional( bool justElse ) 
{
    std::string stmtType = justElse ? "else" : "if";
    Logger::debug( "Parsing " + stmtType + " statement" );

    auto front = m_tokenStream.peek();

    if ( !front.checkTypeMatches( TokenKind::Keyword )) {
        return std::unexpected( UnexpectedTypeError( TokenKind::Keyword, front.getType(), front.getLocation() ) );
    }

    auto keyword = m_tokenStream.consume();

    std::unique_ptr<Expression> condition;

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

        condition = std::move( maybeCondition.value() );

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

    std::unique_ptr<IfConditional> elseStatement;

    auto next = m_tokenStream.peek();

    if ( next.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( next.getLocation() ) );
    }
    
    if ( next.checkMatches( TokenKind::Keyword, TokenKeyword::Else ) ) 
    {
        Logger::trace( "Detected 'else', checking for 'if' to parse else-if or else body" );

        next = m_tokenStream.peek();

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
            elseStatement = std::move( maybeElseStatement.value() );
        } 
        else {
            Logger::trace( "Detected 'else' branch, parsing recursively" );

            auto maybeElseStatement = parseIfConditional( true );
            if ( !maybeElseStatement ) return std::unexpected( maybeElseStatement.error() );
            elseStatement = std::move( maybeElseStatement.value() );
        }
    }

    auto endLocation = elseStatement == nullptr ? maybeBody.value()->location.end : elseStatement->location.end;

    auto ifStmt = std::make_unique<IfConditional>( 
        std::move( condition ), 
        std::move( elseStatement ), 
        std::move( maybeBody.value() ) 
    );

    ifStmt->location = { front.getLocation().start, endLocation, front.getLocation().fileId };

    Logger::debug( "Successfully parsed " + stmtType + " statement" );

    return ifStmt;
}

std::expected<std::unique_ptr<ForLoop>, ErrorVariant> StatementParser::parseForLoop() 
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

    std::unique_ptr<Expression> step;
    std::unique_ptr<Expression> where;

    auto current = m_tokenStream.peek();

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
            step = std::move( maybeStep.value() );
        }

        current = m_tokenStream.peek();

        if ( current.checkTypeMatches( TokenKind::EndOfFile )) {
            return std::unexpected( UnexpectedEndOfInputError( current.getLocation() ) );
        }

        if ( current.checkMatches( TokenKind::Keyword, TokenKeyword::Where ) ) 
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
                where = std::move( maybeWhere.value() );
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
            where = std::move( maybeWhere.value() );
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

    auto forLoop = std::make_unique<ForLoop>( 
        std::move( maybeLoopVar.value() ), 
        std::move( maybeIterable.value() ), 
        std::move( step ), 
        std::move( where ), 
        std::move( maybeBody.value() ) 
    );

    forLoop->location = { maybeForKeyword.value().get().getLocation().start, forLoop->body->location.end, forLoop->body->location.fileId };

    Logger::debug( "Successfully parsed for loop statement" );

    return forLoop;
}