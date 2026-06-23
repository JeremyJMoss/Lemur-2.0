#include "Parser/ExpressionParser.hpp"
#include "AST/BinaryExpression.hpp"
#include "AST/Unary.hpp"
#include <array>

/* === Static Private Member Variables === */

const std::regex ExpressionParser::s_RE_STRING_REPL( ( R"(\\\")" ) );

/* === Public Member Methods === */

std::expected<std::unique_ptr<Expression>, ErrorVariant> ExpressionParser::parseExpression() 
{
    auto front = m_tokenStream.peek();

    if ( front.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( front.getLocation() ) );
    }
    
    auto maybeLeft = parseUnary();
    if ( !maybeLeft ) return std::unexpected( maybeLeft.error() );
    auto left = std::move( maybeLeft.value() );

    while ( true ) 
    {
        auto current = m_tokenStream.peek();

        if ( current.checkTypeMatches( TokenKind::EndOfFile )) {
            return std::unexpected( UnexpectedEndOfInputError( current.getLocation() ) );
        }

        if ( current.checkMatches( TokenKind::Keyword, std::vector<TokenKeyword> { TokenKeyword::To, TokenKeyword::Until } ) ) 
        {
            return parseRange( std::move( left ) );
        }
        
        if ( !current.checkTypeMatches( TokenKind::Symbol ) ) break;

        std::size_t nextPrecedence = m_utils.getPrecedence( current.getSymbol() );

        if ( nextPrecedence <= m_precedence ) break;

        if (!m_tokenStream.peek().checkTypeMatches(TokenKind::Symbol)){
            return std::unexpected( 
                CompilerError(
                    "Missing binary operator in expression.",
                    ErrorSeverity::Error,
                    m_tokenStream.peek().getLocation(),
                    ErrorCategory::Syntax
                )
            );
        }

        auto op = m_tokenStream.consume().getValue();
        
        auto maybeRight = parseUnary();
        if ( !maybeRight ) return std::unexpected( maybeRight.error() );

        auto right = std::move( maybeRight.value() );

        right->location = {front.getLocation().start, right->location.end, front.getLocation().fileId};

        left = std::make_unique<BinaryExpression>( std::move( left ), op, std::move( right ) );

        left->location = {front.getLocation().start, left->location.end, front.getLocation().fileId };
    }

    return left;
}

std::expected<std::unique_ptr<Expression>, ErrorVariant> ExpressionParser::parsePostFixExpression()
{
    auto maybeExpression = parsePrimaryLiteral();
    if ( !maybeExpression ) return std::unexpected( maybeExpression.error() );

    auto expr = std::move( maybeExpression.value() );
    
     // Parse the chain of postfix operations
     while ( true ) 
     {
        auto current = m_tokenStream.peek();

        if ( current.checkTypeMatches( TokenKind::EndOfFile )) {
            return std::unexpected( UnexpectedEndOfInputError( current.getLocation() ) );
        }
        
        if ( current.checkValueMatches( TokenSymbol::LParens ) ) 
        {
            // Function call
            auto maybeParams = parseFunctionCallParams();
            if ( !maybeParams ) return std::unexpected( maybeParams.error() );

            auto exprLocation = expr->location.end;

            expr = std::make_unique<FunctionCall>( std::move( expr ), std::move( maybeParams.value() ) );

            expr->location = {current.getLocation().start, exprLocation, current.getLocation().fileId };
        }
        else 
        {
            // No more postfix operations
            break;
        }
    }

    return expr;
}

std::expected<std::unique_ptr<Expression>, ErrorVariant> ExpressionParser::parsePrimaryLiteral() 
{
    auto front = m_tokenStream.peek();

    if ( front.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( front.getLocation() ) );
    }

    // Handle parenthesized expressions
    if ( front.checkMatches( TokenKind::Symbol, TokenSymbol::LParens ) ) 
    {
        // Peek ahead for function expression pattern
        // We need to look for the pattern '):' after parameters

        std::size_t currentPeekOffset = 0;
        int parenthesisCount = 0;
        bool isFunction = false;

        while ( true ) 
        {
            auto token = m_tokenStream.peek(currentPeekOffset);

            if ( token.checkTypeMatches( TokenKind::EndOfFile )) {
                return std::unexpected( UnexpectedEndOfInputError( token.getLocation() ) );
            }

            if ( token.checkTypeMatches( TokenKind::Symbol ) ) 
            {
                if ( token.checkValueMatches( TokenSymbol::LParens ) ) 
                {
                    parenthesisCount++;
                } 
                else if ( token.checkValueMatches( TokenSymbol::RParens ) ) 
                {
                    if ( --parenthesisCount < 0 )
                    {
                        return std::unexpected(
                            CompilerError(
                                "Unexpected closing parenthesis", 
                                ErrorSeverity::Error,
                                token.getLocation(),
                                ErrorCategory::Syntax
                            )
                        );
                    } 
                } 
                else if ( token.checkValueMatches( TokenSymbol::Colon ) && parenthesisCount == 0 ) 
                {
                    isFunction = true;
                    break;
                }
            }
            
            currentPeekOffset++;
        }

        if ( parenthesisCount > 0 ) 
        {
            return std::unexpected(
                CompilerError(
                    "Unclosed parenthesis in expression", 
                    ErrorSeverity::Error,
                    m_tokenStream.peek(currentPeekOffset).getLocation(),
                    ErrorCategory::Syntax
                )
            );
        }

        if ( isFunction ) 
        {
            return parseFunctionLiteral();
        } 
        else 
        {
            auto maybeOpenParens = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::LParens );
            if ( !maybeOpenParens ) return std::unexpected( maybeOpenParens.error() );

            auto maybeExpression = parseExpression();
            if ( !maybeExpression ) return std::unexpected( maybeExpression.error() );

            auto maybeCloseParens = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::RParens );
            if ( !maybeCloseParens ) return std::unexpected( maybeCloseParens.error() );

            return std::move( maybeExpression.value() );
        }
    }

    if ( front.checkTypeMatches( TokenKind::Identifier ) ) 
    {
        
        auto next = m_tokenStream.peek( 1 );

        if ( next.checkTypeMatches( TokenKind::EndOfFile ) ) {
            return std::unexpected( UnexpectedEndOfInputError( next.getLocation() ) );
        }

        if ( next.checkMatches( TokenKind::Symbol, TokenSymbol::LParens ) ) return parseFunctionCall();

        auto idToken = m_tokenStream.consume();

        auto id = std::make_unique<Identifier>( idToken.getValue() );
        id->location = m_utils.getLocation( idToken );

        return id;
    }

    if ( front.checkMatches( TokenKind::Keyword, TokenKeyword::Null ) ) 
    {
        //TODO implement this
    }

    auto maybeLitValue = getLiteralValue();
    if ( !maybeLitValue ) return std::unexpected( maybeLitValue.error() );

    auto literal = std::make_unique<Literal>( std::move( maybeLitValue.value() ) );
    literal->location = m_utils.getLocation( front );

    return literal;
}

/**
 * Get the literal value based on the current_token
 * 
 * @returns LiteralValue of the primitive type of the current token
 */
std::expected<LiteralValue, ErrorVariant> ExpressionParser::getLiteralValue()
{
    auto current = m_tokenStream.peek();

    if ( current.checkTypeMatches( TokenKind::EndOfFile ) ) {
        return std::unexpected( UnexpectedEndOfInputError( current.getLocation() ) );
    }

    const std::array attrs = {
        Attribute{ "Value", current.getValue() },
        Attribute{ "Location", current.getLocation().toString() }
    };

    // Float Value
    if ( current.checkTypeMatches( TokenKind::Float ) ) 
    {
        Logger::trace( 
            "Consuming Float literal",
            attrs
        );

        auto floatToken = m_tokenStream.consume();

        return std::stof( floatToken.getValue() );
    }

    // Integer value
    if ( current.checkTypeMatches( TokenKind::Integer ) ) 
    {
        Logger::trace( 
            "Consuming Integer literal", 
            attrs
        );

        auto intToken = m_tokenStream.consume();

        return std::stoi( intToken.getValue() );
    }

    // Boolean value
    if ( current.checkTypeMatches( TokenKind::Boolean ) ) 
    {
        Logger::trace( 
            "Consuming Boolean literal",
            attrs
        );

        auto boolToken = m_tokenStream.consume();

        return boolToken.getValue() == "true";
    }

    // Char value
    if ( current.checkTypeMatches( TokenKind::Char ) ) 
    {
        std::string val = current.getValue();

        // Remove surrounding quotes if they exist
        if ( val.size() >= 2 && val.front() == '\'' && val.back() == '\'' ) 
        {
            val = val.substr( 1, val.size() - 2 );
        }

        if ( val.empty() ) 
        {
            return std::unexpected(
                CompilerError(
                    "'char' literal must not be empty",
                    ErrorSeverity::Error,
                    current.getLocation(),
                    ErrorCategory::Syntax
                )
            );
        }

        char resultChar;

        if ( val[0] == '\\' ) 
        {
            if ( val.size() != 2 ) 
            {
                return std::unexpected( 
                    CompilerError(
                        "Invalid escape sequence in 'char'", 
                        ErrorSeverity::Error,
                        current.getLocation(),
                        ErrorCategory::Syntax
                    )
                );
            }

            switch ( val[1] ) 
            {
                case 'n':  resultChar = '\n'; break;
                case 't':  resultChar = '\t'; break;
                case 'r':  resultChar = '\r'; break;
                case '\\': resultChar = '\\'; break;
                case '\'': resultChar = '\''; break;
                case '"':  resultChar = '\"'; break;
                case '0':  resultChar = '\0'; break;
                default: 
                    return std::unexpected( 
                        CompilerError(
                            "Unknown escape sequence in 'char'",
                            ErrorSeverity::Error,
                            current.getLocation(),
                            ErrorCategory::Syntax
                        )
                    );
            }
        } 
        else 
        {
            if ( val.size() != 1 ) 
            {
                return std::unexpected(
                    CompilerError(
                        "'char' literal must be a single character", 
                        ErrorSeverity::Error,
                        current.getLocation(),
                        ErrorCategory::Syntax
                    )
                );
            }

            resultChar = val[0];
        }

        Logger::trace( 
            "Consuming Char literal",
            attrs
        );

        auto charToken = m_tokenStream.consume();

        return resultChar;
    }


    // String value
    if ( current.checkTypeMatches( TokenKind::String ) ) 
    {
        Logger::trace( 
            "Consuming String literal",
            attrs
        );

        auto strToken = m_tokenStream.consume();

        std::string raw = strToken.getValue();
        raw = raw.substr(1, raw.length() - 2);
        raw = std::regex_replace( raw, s_RE_STRING_REPL, "\"" );
        return raw;
    }

    // Nullptr value
    if ( current.checkMatches( TokenKind::Keyword, TokenKeyword::Null ) ) 
    {
        Logger::trace( 
            "Consuming Null literal",
            attrs
        );

        auto keyword = m_tokenStream.consume();

        return std::monostate{};
    }
    
    return std::unexpected( 
        CompilerError(
            "Expected number, boolean, char or string literal, got '" + current.getValue() + "'", 
            ErrorSeverity::Error,
            current.getLocation(),
            ErrorCategory::Syntax 
        )
    );
}

std::expected<std::unique_ptr<Assignment>, ErrorVariant> ExpressionParser::parseAssignment() 
{
    auto front = m_tokenStream.peek();

    if ( front.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( front.getLocation() ) );
    }

    auto maybeLExpression = parsePostFixExpression();
    if ( !maybeLExpression ) return std::unexpected( maybeLExpression.error() );

    auto maybeAssign = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Assign );
    if ( !maybeAssign ) return std::unexpected( maybeAssign.error() );

    auto maybeRExpression = parseExpression();
    if ( !maybeRExpression ) return std::unexpected( maybeRExpression.error() );

    auto assign = std::make_unique<Assignment>( std::move( maybeLExpression.value() ), std::move( maybeRExpression.value() ) );

    assign->location = {front.getLocation().start, assign->value->location.end, front.getLocation().fileId };

    return assign;
}

std::expected<std::unique_ptr<Expression>, ErrorVariant> ExpressionParser::parseUnary() 
{
    auto front = m_tokenStream.peek();
   
    if ( front.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( front.getLocation() ) );
    }

    if ( front.checkMatches( TokenKind::Symbol, std::vector<TokenSymbol> { TokenSymbol::Minus, TokenSymbol::Not } ) ) 
    {
        auto op = m_tokenStream.consume().getValue();

        auto maybeUnary = parseUnary();
        if ( !maybeUnary ) return std::unexpected( maybeUnary.error() );

        auto unexp = std::make_unique<Unary>( std::move( maybeUnary.value() ), op );

        unexp->location = { front.getLocation().start, unexp->argument->location.end, front.getLocation().fileId };

        return unexp;
    }

    // else parse literal or parentheses
    return parsePostFixExpression();
}

std::expected<std::unique_ptr<Range>, ErrorVariant> ExpressionParser::parseRange( std::unique_ptr<Expression>&& start ) 
{
    bool inclusive;

    auto front = m_tokenStream.peek();

    if ( front.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( front.getLocation() ) );
    }

    if ( front.checkValueMatches( TokenKeyword::To ) ) 
    {
        inclusive = true;
    } 
    else if ( front.checkValueMatches( TokenKeyword::Until ) ) 
    {
        inclusive = false;
    } 
    else 
    {
        return std::unexpected(
            CompilerError(
                "Expected either 'to' or 'until' got " + front.getValue(), 
                ErrorSeverity::Error,
                front.getLocation(),
                ErrorCategory::Syntax
            ) 
        );
    }

    auto keyword = m_tokenStream.consume();

    auto maybeEnd = parseExpression();
    if ( !maybeEnd )
    {
        return std::unexpected(
            maybeEnd.error()
        );
    }
    
    auto rngExp = std::make_unique<Range>( 
        std::move( start ), 
        std::move( maybeEnd.value() ), 
        inclusive 
    );

    rngExp->location = { front.getLocation().start, rngExp->end->location.end, front.getLocation().fileId };

    return rngExp;
}

std::expected<std::vector<std::unique_ptr<Expression>>, ErrorVariant> ExpressionParser::parseFunctionCallParams() 
{
    std::vector<std::unique_ptr<Expression>> params;

    auto maybeFrontParens = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::LParens );
    if ( !maybeFrontParens ) return std::unexpected( maybeFrontParens.error() );

    while( true ) {
        auto current = m_tokenStream.peek();

        if ( current.checkTypeMatches( TokenKind::EndOfFile )) {
            return std::unexpected( UnexpectedEndOfInputError( current.getLocation() ) );
        }

        if ( current.checkMatches( TokenKind::Symbol, TokenSymbol::RParens ) ) 
        {
            auto maybeClosingParens = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::RParens );
            if ( !maybeClosingParens ) return std::unexpected( maybeClosingParens.error() );
            break;
        }

        auto maybeParameter = parseExpression();
        if ( !maybeParameter ) return std::unexpected( maybeParameter.error() );
        
        params.emplace_back( std::move( maybeParameter.value() ) );

        auto next = m_tokenStream.peek();

        if ( next.checkTypeMatches( TokenKind::EndOfFile )) {
            return std::unexpected( UnexpectedEndOfInputError( next.getLocation() ) );
        }

        if (next.checkTypeMatches( TokenKind::Symbol ) ) 
        {
            if ( next.checkValueMatches( TokenSymbol::RParens ) ) continue;
            auto maybeComma = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Comma );
            if ( !maybeComma ) return std::unexpected( maybeComma.error() );
        }
    }

    return params;
}

std::expected<std::unique_ptr<FunctionCall>, ErrorVariant> ExpressionParser::parseFunctionCall() 
{
    auto front = m_tokenStream.peek();

    if ( !front.checkTypeMatches( TokenKind::Identifier )) {
        return std::unexpected( UnexpectedTypeError( TokenKind::Identifier, front.getType(), front.getLocation() ) );
    }

    auto idToken = m_tokenStream.consume();

    auto identifier = std::make_unique<Identifier>( idToken.getValue() );
    identifier->location = m_utils.getLocation( idToken );

    auto maybeFunctionCallParams = parseFunctionCallParams();
    if ( !maybeFunctionCallParams ) return std::unexpected( maybeFunctionCallParams.error() );

    auto funCall = std::make_unique<FunctionCall>( std::move( identifier ), std::move( maybeFunctionCallParams.value() ) );

    funCall->location = { front.getLocation().start, funCall->params.back()->location.end, front.getLocation().fileId };

    return funCall;
}

std::expected<std::unique_ptr<FunctionLiteral>, ErrorVariant> ExpressionParser::parseFunctionLiteral() 
{
    auto front = m_tokenStream.peek();

    if ( front.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( front.getLocation() ) );
    }

    auto maybeParameters = m_paramParser.parseFunctionParameters();
    if ( !maybeParameters ) return std::unexpected( maybeParameters.error() );

    auto maybeColon = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Colon );
    if ( !maybeColon ) return std::unexpected( maybeColon.error() );

    auto maybeReturnType = m_typeParser.parseType();
    if ( !maybeReturnType ) return std::unexpected( maybeReturnType.error() );

    auto current = m_tokenStream.peek();

    if ( current.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( current.getLocation() ) );
    }

    // after function expressions expect >> before body
    if ( current.checkMatches( TokenKind::Symbol, TokenSymbol::Greater ) ) 
    {
        auto maybeFirstAngle = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Greater );
        if ( !maybeFirstAngle ) return std::unexpected( maybeFirstAngle.error() );

        auto next = m_tokenStream.peek();

        if ( next.checkTypeMatches( TokenKind::EndOfFile )) {
            return std::unexpected( UnexpectedEndOfInputError( next.getLocation() ) );
        }

        // check if the > is directly preceded by another >
        if ( next.checkMatches( TokenKind::Symbol, TokenSymbol::Greater ) && next.getLocation().start.line == current.getLocation().start.line 
            && next.getLocation().start.column == current.getLocation().start.column + 1 ) 
        {
            auto maybeSecondAngle = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Greater );
            if ( !maybeSecondAngle ) return std::unexpected( maybeSecondAngle.error() );               
        } 
        else 
        {
            return std::unexpected( 
                CompilerError(
                    "Expected '>>' after return value in function expression", 
                    ErrorSeverity::Error,
                    current.getLocation(),
                    ErrorCategory::Syntax
                )
            );
        }
    } 
    else 
    {
        return std::unexpected( 
            CompilerError(
                "Expected '>>' after return value in function expression", 
                ErrorSeverity::Error,
                current.getLocation(),
                ErrorCategory::Syntax
            )
        );
    }

    auto maybeBody = m_stmtParser->parseBlock();
    if ( !maybeBody )
    {
        return std::unexpected(
            CompilerError(
                "Function Expressions must have function body",
                ErrorSeverity::Error,
                current.getLocation(),
                ErrorCategory::Syntax
            )
        );
    }

    auto funExpr = std::make_unique<FunctionLiteral>( 
        std::move( maybeReturnType.value() ), 
        std::move( maybeParameters.value() ), 
        std::move( maybeBody.value() ) 
    );

    funExpr->location = { front.getLocation().start, funExpr->body->location.end, front.getLocation().fileId };

    return funExpr;
}

std::expected<std::unique_ptr<Expression>, ErrorVariant> ExpressionParser::parseInitialiser() {
    auto maybeAssignToken = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Assign );
    if ( !maybeAssignToken ) return std::unexpected( maybeAssignToken.error() );

    auto maybeExpression = parseExpression();
    return maybeExpression;
}