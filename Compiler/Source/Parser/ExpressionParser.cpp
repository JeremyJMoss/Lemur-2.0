#include <array>

#include "Parser/ExpressionParser.hpp"
#include "Tokens/TokenStream.hpp"
#include "Driver/CompilationUnit.hpp"
#include "Parser/ParameterParser.hpp"
#include "Parser/TypeParser.hpp"
#include "Parser/StatementParser.hpp"
#include "AST/Block.hpp"
#include "AST/BinaryExpression.hpp"
#include "AST/Unary.hpp"
#include "AST/Range.hpp"
#include "AST/FunctionCall.hpp"
#include "AST/FunctionLiteral.hpp"
#include "AST/Identifier.hpp"
#include "AST/Literal.hpp"
#include "AST/Assignment.hpp"
#include "Utils/Logger.hpp"

/* === Helper Methods === */

size_t ExpressionParser::getPrecedence( TokenSymbol op ) 
{
    size_t prec = 0;

    switch (op){
        case TokenSymbol::Or:
            prec = 1;
            break;
        case TokenSymbol::And:
            prec = 2;
            break;
        case TokenSymbol::Equals:
        case TokenSymbol::NotEquals:
            prec = 3;
            break;
        case TokenSymbol::Less:
        case TokenSymbol::Greater:
        case TokenSymbol::LessEquals:
        case TokenSymbol::GreaterEquals:
            prec = 4;
            break;
        case TokenSymbol::Plus:
        case TokenSymbol::Minus:
            prec = 5;
            break;
        case TokenSymbol::Star:
        case TokenSymbol::Slash:
        case TokenSymbol::Percent:
            prec = 6;
            break;
        default:
            break;
    }

    Logger::trace( "Operator '" + toString( op ) + "' has precedence " + std::to_string( prec ) );
    return prec;
}

std::string ExpressionParser::unescapeString( std::string_view raw )
{
    std::string out;
    out.reserve( raw.size() );

    for ( size_t i = 0; i < raw.size(); ++i )
    {
        if ( raw[i] == '\\' && i + 1 < raw.size() )
        {
            switch ( raw[i + 1] )
            {
                case '"':
                    out.push_back( '"' );
                    ++i;
                    break;

                case '\\':
                    out.push_back( '\\' );
                    ++i;
                    break;

                case 'n':
                    out.push_back( '\n' );
                    ++i;
                    break;

                case 't':
                    out.push_back( '\t' );
                    ++i;
                    break;

                default:
                    // unknown escape — keep literal or handle error
                    out.push_back( raw[i + 1] );
                    ++i;
                    break;
            }
        }
        else
        {
            out.push_back( raw[i] );
        }
    }

    return out;
}

/* === Public Member Methods === */

std::expected<Expression*, ErrorVariant> ExpressionParser::parseExpression(std::size_t min_precedence) 
{
    const Token& front = m_tokenStream.peek();

    if ( front.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( front.getLocation() ) );
    }
    
    auto maybeLeft = parseUnary();
    if ( !maybeLeft ) return std::unexpected( maybeLeft.error() );
    auto left = std::move( maybeLeft.value() );

    while ( true ) 
    {
        const Token& current = m_tokenStream.peek();

        if ( current.checkTypeMatches( TokenKind::EndOfFile )) {
            return std::unexpected( UnexpectedEndOfInputError( current.getLocation() ) );
        }

        if ( current.checkMatches( TokenKind::Keyword, std::vector<TokenKeyword> { TokenKeyword::To, TokenKeyword::Until } ) ) 
        {
            return parseRange( left );
        }
        
        if ( !current.checkTypeMatches( TokenKind::Symbol ) ) break;

        std::size_t nextPrecedence = getPrecedence( current.getSymbol() );

        if ( nextPrecedence <= min_precedence ) break;

        auto op = m_tokenStream.consume().getValue();
        
        auto maybeRight = parseExpression( nextPrecedence + 1 );
        if ( !maybeRight ) return std::unexpected( maybeRight.error() );

        auto right = maybeRight.value();

        right->location = { front.getLocation().start, right->location.end, front.getLocation().fileId };

        left = m_compUnit.allocate<BinaryExpression>( left, op, right );

        left->location = { front.getLocation().start, left->location.end, front.getLocation().fileId };
    }

    return left;
}

std::expected<Expression*, ErrorVariant> ExpressionParser::parsePostFixExpression()
{
    auto maybeExpression = parsePrimaryLiteral();
    if ( !maybeExpression ) return std::unexpected( maybeExpression.error() );

    auto expr = std::move( maybeExpression.value() );
    
     // Parse the chain of postfix operations
     while ( true ) 
     {
        const Token& current = m_tokenStream.peek();

        if ( current.checkTypeMatches( TokenKind::EndOfFile )) {
            return std::unexpected( UnexpectedEndOfInputError( current.getLocation() ) );
        }
        
        if ( current.checkValueMatches( TokenSymbol::LParens ) ) 
        {
            // Function call
            auto maybeParams = parseFunctionCallArgs();
            if ( !maybeParams ) return std::unexpected( maybeParams.error() );

            auto exprLocation = expr->location.end;

            expr = m_compUnit.allocate<FunctionCall>( expr, maybeParams.value() );

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

std::expected<Expression*, ErrorVariant> ExpressionParser::parsePrimaryLiteral() 
{
    const Token& front = m_tokenStream.peek();

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
            const Token& token = m_tokenStream.peek(currentPeekOffset);

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

            return maybeExpression.value();
        }
    }

    if ( front.checkTypeMatches( TokenKind::Identifier ) ) 
    {
        const Token& next = m_tokenStream.peek( 1 );

        if ( next.checkTypeMatches( TokenKind::EndOfFile ) ) {
            return std::unexpected( UnexpectedEndOfInputError( next.getLocation() ) );
        }

        if ( next.checkMatches( TokenKind::Symbol, TokenSymbol::LParens ) ) {
            return parseFunctionCall();
        }

        const Token& idToken = m_tokenStream.consume();

        auto id = m_compUnit.allocate<Identifier>( idToken.getValue() );
        id->location = SourceRange::getLocation( idToken );

        return id;
    }

    if ( front.checkMatches( TokenKind::Keyword, TokenKeyword::Null ) ) 
    {
        //TODO implement this
    }

    auto maybeLitValue = getLiteralValue();
    if ( !maybeLitValue ) return std::unexpected( maybeLitValue.error() );

    auto literal = m_compUnit.allocate<Literal>( maybeLitValue.value() );
    literal->location = SourceRange::getLocation( front );

    return literal;
}

/**
 * Get the literal value based on the current_token
 * 
 * @returns LiteralValue of the primitive type of the current token
 */
std::expected<LiteralValue, ErrorVariant> ExpressionParser::getLiteralValue()
{
    const Token& current = m_tokenStream.peek();

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

        const Token& floatToken = m_tokenStream.consume();

        return std::stof( floatToken.getValue() );
    }

    // Integer value
    if ( current.checkTypeMatches( TokenKind::Integer ) ) 
    {
        Logger::trace( 
            "Consuming Integer literal", 
            attrs
        );

        const Token& intToken = m_tokenStream.consume();

        return std::stoi( intToken.getValue() );
    }

    // Boolean value
    if ( current.checkTypeMatches( TokenKind::Boolean ) ) 
    {
        Logger::trace( 
            "Consuming Boolean literal",
            attrs
        );

        const Token& boolToken = m_tokenStream.consume();

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

        m_tokenStream.consume();

        return resultChar;
    }


    // String value
    if ( current.checkTypeMatches( TokenKind::String ) ) 
    {
        Logger::trace( 
            "Consuming String literal",
            attrs
        );

        const Token& strToken = m_tokenStream.consume();

        std::string raw = strToken.getValue();
        raw = raw.substr(1, raw.length() - 2);
        return unescapeString( raw );
    }

    // Nullptr value
    if ( current.checkMatches( TokenKind::Keyword, TokenKeyword::Null ) ) 
    {
        Logger::trace( 
            "Consuming Null literal",
            attrs
        );

        m_tokenStream.consume();

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

std::expected<Assignment*, ErrorVariant> ExpressionParser::parseAssignment() 
{
    const Token& front = m_tokenStream.peek();

    if ( front.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( front.getLocation() ) );
    }

    auto maybeLExpression = parsePostFixExpression();
    if ( !maybeLExpression ) return std::unexpected( maybeLExpression.error() );

    auto maybeAssign = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Assign );
    if ( !maybeAssign ) return std::unexpected( maybeAssign.error() );

    auto maybeRExpression = parseExpression();
    if ( !maybeRExpression ) return std::unexpected( maybeRExpression.error() );

    auto assign = m_compUnit.allocate<Assignment>( maybeLExpression.value(), maybeRExpression.value() );

    assign->location = { front.getLocation().start, assign->value->location.end, front.getLocation().fileId };

    return assign;
}

std::expected<Expression*, ErrorVariant> ExpressionParser::parseUnary() 
{
    const Token& front = m_tokenStream.peek();
   
    if ( front.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( front.getLocation() ) );
    }

    if ( front.checkMatches( TokenKind::Symbol, std::vector<TokenSymbol> { TokenSymbol::Minus, TokenSymbol::Not } ) ) 
    {
        auto op = m_tokenStream.consume().getValue();

        auto maybeUnary = parseUnary();
        if ( !maybeUnary ) return std::unexpected( maybeUnary.error() );

        auto unexp = m_compUnit.allocate<Unary>( maybeUnary.value(), op );

        unexp->location = { front.getLocation().start, unexp->argument->location.end, front.getLocation().fileId };

        return unexp;
    }

    // else parse literal or parentheses
    return parsePostFixExpression();
}

std::expected<Range*, ErrorVariant> ExpressionParser::parseRange( Expression* start ) 
{
    bool inclusive;

    const Token& front = m_tokenStream.peek();

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

    m_tokenStream.consume();

    auto maybeEnd = parseExpression();
    if ( !maybeEnd )
    {
        return std::unexpected(
            maybeEnd.error()
        );
    }
    
    auto rngExp = m_compUnit.allocate<Range>( 
        start, 
        maybeEnd.value(), 
        inclusive 
    );

    rngExp->location = { front.getLocation().start, rngExp->end->location.end, front.getLocation().fileId };

    return rngExp;
}

std::expected<std::vector<Expression*>, ErrorVariant> ExpressionParser::parseFunctionCallArgs() 
{
    std::vector<Expression*> params;

    auto maybeFrontParens = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::LParens );
    if ( !maybeFrontParens ) return std::unexpected( maybeFrontParens.error() );

    while( true ) {
        const Token& current = m_tokenStream.peek();

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
        
        params.emplace_back( maybeParameter.value() );

        const Token& next = m_tokenStream.peek();

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

std::expected<FunctionCall*, ErrorVariant> ExpressionParser::parseFunctionCall() 
{
    Logger::debug( "Parsing function call" );

    const Token& front = m_tokenStream.peek();

    if ( !front.checkTypeMatches( TokenKind::Identifier ) ) {
        return std::unexpected( UnexpectedTypeError( TokenKind::Identifier, front.getType(), front.getLocation() ) );
    }

    const Token& idToken = m_tokenStream.consume();

    auto identifier = m_compUnit.allocate<Identifier>( idToken.getValue() );
    identifier->location = SourceRange::getLocation( idToken );

    auto maybeFunctionCallParams = parseFunctionCallArgs();
    if ( !maybeFunctionCallParams ) return std::unexpected( maybeFunctionCallParams.error() );

    auto funCall = m_compUnit.allocate<FunctionCall>( identifier, maybeFunctionCallParams.value() );

    funCall->location = { front.getLocation().start, funCall->arguments.back()->location.end, front.getLocation().fileId };

    Logger::debug( 
        "Parsed function call",
        std::to_array<Attribute>({
            { "Function Identifier", idToken.getValue() },
            { "Argument Count", std::to_string(funCall->arguments.size()) }
        }) 
    );

    return funCall;
}

std::expected<FunctionLiteral*, ErrorVariant> ExpressionParser::parseFunctionLiteral() 
{
    const Token& front = m_tokenStream.peek();

    if ( front.checkTypeMatches( TokenKind::EndOfFile ) ) {
        return std::unexpected( UnexpectedEndOfInputError( front.getLocation() ) );
    }

    auto maybeParameters = m_paramParser.parseFunctionParameters();
    if ( !maybeParameters ) return std::unexpected( maybeParameters.error() );

    auto maybeColon = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Colon );
    if ( !maybeColon ) return std::unexpected( maybeColon.error() );

    auto maybeReturnType = m_typeParser.parseType();
    if ( !maybeReturnType ) return std::unexpected( maybeReturnType.error() );

    const Token& current = m_tokenStream.peek();

    if ( current.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( UnexpectedEndOfInputError( current.getLocation() ) );
    }

    // after function expressions expect >> before body
    if ( current.checkMatches( TokenKind::Symbol, TokenSymbol::Greater ) ) 
    {
        auto maybeFirstAngle = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Greater );
        if ( !maybeFirstAngle ) return std::unexpected( maybeFirstAngle.error() );

        const Token& next = m_tokenStream.peek();

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

    auto funExpr = m_compUnit.allocate<FunctionLiteral>( 
        maybeReturnType.value(),
        maybeParameters.value(),
        maybeBody.value() 
    );

    funExpr->location = { front.getLocation().start, funExpr->body->location.end, front.getLocation().fileId };

    return funExpr;
}

std::expected<Expression*, ErrorVariant> ExpressionParser::parseInitialiser() {
    auto maybeAssignToken = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Assign );
    if ( !maybeAssignToken ) return std::unexpected( maybeAssignToken.error() );

    auto maybeExpression = parseExpression();
    return maybeExpression;
}