#include "Parser/ExpressionParser.hpp"
#include "AST/BinaryOperation.hpp"
#include "AST/Unary.hpp"
#include <array>

/* === Static Private Member Variables === */

const std::regex ExpressionParser::s_RE_STRING_REPL( ( R"(\\\")" ) );

/* === Public Member Methods === */

std::expected<std::unique_ptr<Expression>, ErrorVariant> ExpressionParser::parseExpression() 
{
    auto maybeFrontToken = m_utils.peek();
    if ( !maybeFrontToken ) return std::unexpected( maybeFrontToken.error() );
    const Token& front = maybeFrontToken.value();
    
    auto maybeLeft = parseUnary();
    if ( !maybeLeft ) return std::unexpected( maybeLeft.error() );
    auto left = std::move( maybeLeft.value() );

    while ( true ) 
    {
        auto maybeCurrentToken = m_utils.peek();
        if ( !maybeCurrentToken ) return std::unexpected( maybeCurrentToken.error() );
        const Token& current = maybeCurrentToken.value();

        if ( current.checkMatches( TokenKind::Keyword, std::vector<TokenKeyword> { TokenKeyword::To, TokenKeyword::Until } ) ) 
        {
            return parseRange( std::move( left ) );
        }
        
        if ( !current.checkTypeMatches( TokenKind::Symbol ) ) break;

        size_t nextPrecedence = m_utils.getPrecedence( current.getSymbol() );

        if ( nextPrecedence <= m_precedence ) break;

        auto maybeOperator = m_utils.consume( TokenKind::Symbol );
        if ( !maybeOperator ) 
        {
            auto maybePrevToken = m_utils.peekBack();
            if ( !maybePrevToken ) return std::unexpected( maybePrevToken.error() );

            return std::unexpected( 
                CompilerError(
                    ErrorSeverity::Error,
                    "Missing binary operator in expression.",
                    maybePrevToken.value().getLocation(),
                    ErrorCategory::Syntax
                )
            );
        }

        const std::string& op = maybeOperator.value().getValue();
        
        auto maybeRight = parseUnary();
        if ( !maybeRight ) return std::unexpected( maybeRight.error() );

        auto right = std::move( maybeRight.value() );

        left = std::make_unique<BinaryOperation>( std::move( left ), op, std::move( right ) );

        auto maybeEndToken = m_utils.peekBack();
        if ( !maybeEndToken ) return std::unexpected( maybeEndToken.error() );

        left->location = m_utils.getLocation( front, maybeEndToken.value() );
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
        auto maybeNextToken = m_utils.peek();
        if ( !maybeNextToken ) return std::unexpected( maybeNextToken.error() );

        const Token& next = maybeNextToken.value();
        
        if ( next.checkValueMatches( TokenSymbol::LParens ) ) 
        {
            // Function call
            auto maybeParams = parseFunctionCallParams();
            if ( !maybeParams ) return std::unexpected( maybeParams.error() );

            expr = std::make_unique<FunctionCall>( std::move( expr ), std::move( maybeParams.value() ) );

            auto maybeEndToken = m_utils.peekBack();
            if ( !maybeEndToken ) return std::unexpected( maybeEndToken.error() );

            expr->location = m_utils.getLocation( next, maybeEndToken.value() );
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
    auto maybeFrontToken = m_utils.peek();
    if ( !maybeFrontToken ) return std::unexpected( maybeFrontToken.error() );
    const Token& front = maybeFrontToken.value();

    // Handle parenthesized expressions
    if ( front.checkMatches( TokenKind::Symbol, TokenSymbol::LParens ) ) 
    {
        // Peek ahead for function expression pattern
        // We need to look for the pattern '):' after parameters

        size_t pos = m_utils.getCurrentPosition();
        int parenthesisCount = 0;
        bool isFunction = false;

        const auto& tokens = m_utils.getTokens();

        Token token;
        while ( pos < tokens.size() ) 
        {
            token = tokens.at( pos );

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
                                ErrorSeverity::Error,
                                "Unexpected closing parenthesis", 
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
            
            pos++;
        }

        if ( parenthesisCount > 0 ) 
        {
            return std::unexpected(
                CompilerError(
                    ErrorSeverity::Error,
                    "Unclosed parenthesis in expression", 
                    tokens.at( pos - 1 ).getLocation(),
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
            auto maybeOpenParens = m_utils.expect( TokenKind::Symbol, TokenSymbol::LParens );
            if ( !maybeOpenParens ) return std::unexpected( maybeOpenParens.error() );

            auto maybeExpression = parseExpression();
            if ( !maybeExpression ) return std::unexpected( maybeExpression.error() );

            auto maybeCloseParens = m_utils.expect( TokenKind::Symbol, TokenSymbol::RParens );
            if ( !maybeCloseParens ) return std::unexpected( maybeCloseParens.error() );

            return std::move( maybeExpression.value() );
        }
    }

    if ( front.checkTypeMatches( TokenKind::Identifier ) ) 
    {
        
        auto maybeNextToken = m_utils.peek( 2 );

        if ( maybeNextToken )
        {
            if ( maybeNextToken.value().checkMatches( TokenKind::Symbol, TokenSymbol::LParens ) ) return parseFunctionCall();
        }

        auto maybeIdToken = m_utils.consume( TokenKind::Identifier );
        if ( !maybeIdToken ) return std::unexpected( maybeIdToken.error() );
        const Token& idToken = maybeIdToken.value();

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
    auto maybeToken = m_utils.peek();
    if ( !maybeToken ) 
    {
        Logger::trace( "EOF reached while expecting a literal" );
        return std::unexpected( maybeToken.error() );
    }

    const Token& current = maybeToken.value();

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

        auto maybeFloatToken = m_utils.consume( TokenKind::Float );
        if ( !maybeFloatToken ) return std::unexpected( maybeFloatToken.error() );

        const Token& floatToken = maybeFloatToken.value();

        return std::stof( floatToken.getValue() );
    }

    // Integer value
    if ( current.checkTypeMatches( TokenKind::Integer ) ) 
    {
        Logger::trace( 
            "Consuming Integer literal", 
            attrs
        );

        auto maybeIntToken = m_utils.consume( TokenKind::Integer );
        if ( !maybeIntToken ) return std::unexpected( maybeIntToken.error() );

        const Token& intToken = maybeIntToken.value();

        return std::stoi( intToken.getValue() );
    }

    // Boolean value
    if ( current.checkTypeMatches( TokenKind::Boolean ) ) 
    {
        Logger::trace( 
            "Consuming Boolean literal",
            attrs
        );

        auto maybeBoolToken = m_utils.consume( TokenKind::Boolean );
        if ( !maybeBoolToken ) return std::unexpected( maybeBoolToken.error() );

        const Token& boolToken = maybeBoolToken.value();

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
                    ErrorSeverity::Error,
                    "'char' literal must not be empty",
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
                        ErrorSeverity::Error,
                        "Invalid escape sequence in 'char'", 
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
                            ErrorSeverity::Error,
                            "Unknown escape sequence in 'char'",
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
                        ErrorSeverity::Error,
                        "'char' literal must be a single character", 
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

        auto maybeCharToken = m_utils.consume( TokenKind::Char );

        if ( !maybeCharToken ) return std::unexpected( maybeCharToken.error() );

        return resultChar;
    }


    // String value
    if ( current.checkTypeMatches( TokenKind::String ) ) 
    {
        Logger::trace( 
            "Consuming String literal",
            attrs
        );

        auto maybeStrToken = m_utils.consume( TokenKind::String );
        if ( !maybeStrToken ) return std::unexpected( maybeStrToken.error() );

        const Token& strToken = maybeStrToken.value();

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

        auto maybeKeyword = m_utils.consume( TokenKind::Keyword );
        if ( !maybeKeyword ) return std::unexpected( maybeKeyword.error() );

        return std::monostate{};
    }
    
    return std::unexpected( 
        CompilerError(
            ErrorSeverity::Error,
            "Expected number, boolean, char or string literal, got '" + current.getValue() + "'", 
            current.getLocation(),
            ErrorCategory::Syntax 
        )
    );
}

std::expected<std::unique_ptr<Assignment>, ErrorVariant> ExpressionParser::parseAssignment() 
{
    auto maybeFrontToken = m_utils.peek();
    if ( !maybeFrontToken ) return std::unexpected( maybeFrontToken.error() );
    const Token& front = maybeFrontToken.value();

    auto maybeLExpression = parsePostFixExpression();
    if ( !maybeLExpression ) return std::unexpected( maybeLExpression.error() );

    auto maybeAssign = m_utils.expect( TokenKind::Symbol, TokenSymbol::Assign );
    if ( !maybeAssign ) return std::unexpected( maybeAssign.error() );

    auto maybeRExpression = parseExpression();
    if ( !maybeRExpression ) return std::unexpected( maybeRExpression.error() );

    auto assign = std::make_unique<Assignment>( std::move( maybeLExpression.value() ), std::move( maybeRExpression.value() ) );

    auto maybeEndToken = m_utils.peekBack();
    if ( !maybeEndToken ) return std::unexpected( maybeEndToken.error() );

    assign->location = m_utils.getLocation( front, maybeEndToken.value() );

    return assign;
}

std::expected<std::unique_ptr<Expression>, ErrorVariant> ExpressionParser::parseUnary() 
{
    auto maybeFrontToken = m_utils.peek();
    if ( !maybeFrontToken ) return std::unexpected( maybeFrontToken.error() );
    const Token& front = maybeFrontToken.value();

    if ( front.checkMatches( TokenKind::Symbol, std::vector<TokenSymbol> { TokenSymbol::Minus, TokenSymbol::Not } ) ) 
    {
        auto maybeOperator = m_utils.consume( TokenKind::Symbol );
        if ( !maybeOperator ) 
        {
            auto maybePrevToken = m_utils.peekBack();
            if ( !maybePrevToken ) return std::unexpected( maybePrevToken.error() );

            return std::unexpected( 
                CompilerError(
                    ErrorSeverity::Error,
                    "Missing unary operator in expression.",
                    maybePrevToken.value().getLocation(),
                    ErrorCategory::Syntax
                )
            );
        }

        const std::string& op = maybeOperator.value().getValue();

        auto maybeUnary = parseUnary();
        if ( !maybeUnary ) return std::unexpected( maybeUnary.error() );

        auto unexp = std::make_unique<Unary>( std::move( maybeUnary.value() ), op );

        auto maybeEndToken = m_utils.peekBack();
        if ( !maybeEndToken ) return std::unexpected( maybeEndToken.error() );

        unexp->location = m_utils.getLocation( front, maybeEndToken.value() );

        return unexp;
    }

    // else parse literal or parentheses
    return parsePostFixExpression();
}

std::expected<std::unique_ptr<Range>, ErrorVariant> ExpressionParser::parseRange( std::unique_ptr<Expression>&& start ) 
{
    bool inclusive;

    auto maybeFrontToken = m_utils.peek();
    if ( !maybeFrontToken ) return std::unexpected( maybeFrontToken.error() );
    const Token& front = maybeFrontToken.value();

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
                ErrorSeverity::Error,
                "Expected either 'to' or 'until' got " + front.getValue(), 
                front.getLocation(),
                ErrorCategory::Syntax
            ) 
        );
    }

    auto maybeKeyword = m_utils.consume( TokenKind::Keyword );
    if ( !maybeKeyword ) return std::unexpected( maybeKeyword.error() );

    auto maybeEnd = parseExpression();
    if ( !maybeEnd )
    {
        auto maybeIssueToken = m_utils.peekBack();
        if ( !maybeIssueToken ) return std::unexpected( maybeIssueToken.error() );

        return std::unexpected(
            CompilerError(
                ErrorSeverity:: Error,
                "End of range is either malformed or missing",
                maybeIssueToken.value().getLocation(),
                ErrorCategory::Syntax
            )
        );
    }

    auto maybeEndToken = m_utils.peekBack();
    if ( !maybeEndToken ) return std::unexpected( maybeEndToken.error() );
    
    auto rngExp = std::make_unique<Range>( 
        std::move( start ), 
        std::move( maybeEnd.value() ), 
        inclusive 
    );

    rngExp->location = m_utils.getLocation( front, maybeEndToken.value() );

    return rngExp;
}

std::expected<std::vector<std::unique_ptr<Expression>>, ErrorVariant> ExpressionParser::parseFunctionCallParams() 
{
    std::vector<std::unique_ptr<Expression>> params;

    auto maybeFrontParens = m_utils.expect( TokenKind::Symbol, TokenSymbol::LParens );
    if ( !maybeFrontParens ) return std::unexpected( maybeFrontParens.error() );

    while( true ) {
        auto maybeCurrentToken = m_utils.peek();
        if ( !maybeCurrentToken ) return std::unexpected( maybeCurrentToken.error() );
        const Token& current = maybeCurrentToken.value();

        if ( current.checkMatches( TokenKind::Symbol, TokenSymbol::RParens ) ) 
        {
            auto maybeClosingParens = m_utils.expect( TokenKind::Symbol, TokenSymbol::RParens );
            if ( !maybeClosingParens ) return std::unexpected( maybeClosingParens.error() );
            break;
        }

        auto maybeParameter = parseExpression();
        if ( !maybeParameter ) return std::unexpected( maybeParameter.error() );
        
        params.emplace_back( std::move( maybeParameter.value() ) );

        auto maybeNextToken = m_utils.peek();
        if ( !maybeNextToken ) return std::unexpected( maybeNextToken.error() );
        const Token& next = maybeNextToken.value();

        if (next.checkTypeMatches( TokenKind::Symbol ) ) 
        {
            if ( next.checkValueMatches( TokenSymbol::RParens ) ) continue;
            auto maybeComma = m_utils.expect( TokenKind::Symbol, TokenSymbol::Comma );
            if ( !maybeComma ) return std::unexpected( maybeComma.error() );
        }
    }

    return params;
}

std::expected<std::unique_ptr<FunctionCall>, ErrorVariant> ExpressionParser::parseFunctionCall() 
{
    auto maybeIdToken = m_utils.consume( TokenKind::Identifier );
    if ( !maybeIdToken ) return std::unexpected( maybeIdToken.error() );
    const Token& idToken = maybeIdToken.value();

    auto identifier = std::make_unique<Identifier>( idToken.getValue() );
    identifier->location = m_utils.getLocation( idToken );

    auto maybeFunctionCallParams = parseFunctionCallParams();
    if ( !maybeFunctionCallParams ) return std::unexpected( maybeFunctionCallParams.error() );

    auto funCall = std::make_unique<FunctionCall>( std::move( identifier ), std::move( maybeFunctionCallParams.value() ) );

    auto maybeEndToken = m_utils.peekBack();
    if ( !maybeEndToken ) return std::unexpected( maybeEndToken.error() );

    funCall->location = m_utils.getLocation( idToken, maybeEndToken.value() );

    return funCall;
}

std::expected<std::unique_ptr<FunctionLiteral>, ErrorVariant> ExpressionParser::parseFunctionLiteral() 
{
    auto maybeFrontToken = m_utils.peek();
    if ( !maybeFrontToken ) return std::unexpected( maybeFrontToken.error() );
    const Token& front = maybeFrontToken.value();

    auto maybeParameters = m_paramParser.parseFunctionParameters();
    if ( !maybeParameters ) return std::unexpected( maybeParameters.error() );

    auto maybeColon = m_utils.expect( TokenKind::Symbol, TokenSymbol::Colon );
    if ( !maybeColon ) return std::unexpected( maybeColon.error() );

    auto maybeReturnType = m_typeParser.parseType();
    if ( !maybeReturnType ) return std::unexpected( maybeReturnType.error() );

    auto maybeCurrentToken = m_utils.peek();
    if ( !maybeCurrentToken ) return std::unexpected( maybeCurrentToken.error() );
    const Token& current = maybeCurrentToken.value();

    // after function expressions expect >> before body
    if ( current.checkMatches( TokenKind::Symbol, TokenSymbol::Greater ) ) 
    {
        auto maybeFirstAngle = m_utils.expect( TokenKind::Symbol, TokenSymbol::Greater );
        if ( !maybeFirstAngle ) return std::unexpected( maybeFirstAngle.error() );

        auto maybeNextToken = m_utils.peek();
        if ( !maybeNextToken ) return std::unexpected( maybeNextToken.error() );
        const Token& next = maybeNextToken.value();

        // check if the > is directly preceded by another >
        if ( next.checkMatches( TokenKind::Symbol, TokenSymbol::Greater ) && next.getLocation().start.line == current.getLocation().start.line 
            && next.getLocation().start.column == current.getLocation().start.column + 1 ) 
        {
            auto maybeSecondAngle = m_utils.expect( TokenKind::Symbol, TokenSymbol::Greater );
            if ( !maybeSecondAngle ) return std::unexpected( maybeSecondAngle.error() );               
        } 
        else 
        {
            return std::unexpected( 
                CompilerError(
                    ErrorSeverity::Error,
                    "Expected '>>' after return value in function expression", 
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
                ErrorSeverity::Error,
                "Expected '>>' after return value in function expression", 
                current.getLocation(),
                ErrorCategory::Syntax
            )
        );
    }

    auto maybeBody = m_stmtParser->parseBlock();
    if ( !maybeBody )
    {
        auto maybeIssueToken = m_utils.peekBack();
        if ( !maybeIssueToken ) return std::unexpected( maybeIssueToken.error() );

        return std::unexpected(
            CompilerError(
                ErrorSeverity::Error,
                "Function Expressions must have function body",
                maybeIssueToken.value().getLocation(),
                ErrorCategory::Syntax
            )
        );
    }

    auto funExpr = std::make_unique<FunctionLiteral>( 
        std::move( maybeReturnType.value() ), 
        std::move( maybeParameters.value() ), 
        std::move( maybeBody.value() ) 
    );

    auto maybeEndToken = m_utils.peekBack();
    if ( !maybeEndToken ) return std::unexpected( maybeEndToken.error() );

    funExpr->location = m_utils.getLocation( front, maybeEndToken.value() );

    return funExpr;
}

std::expected<std::unique_ptr<Expression>, ErrorVariant> ExpressionParser::parseInitialiser() {
    auto maybeAssignToken = m_utils.expect( TokenKind::Symbol, TokenSymbol::Assign );
    if ( !maybeAssignToken ) return std::unexpected( maybeAssignToken.error() );

    auto maybeExpression = parseExpression();
    return maybeExpression;
}