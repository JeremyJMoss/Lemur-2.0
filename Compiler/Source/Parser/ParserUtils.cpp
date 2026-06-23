#include "Parser/ParserUtils.hpp"
#include "Utils/Logger.hpp"

/**
 * Gets the source range of the file based on the tokens passed in
 * 
 * @param const Token& startToken The start to the ast node tokens
 * @param const Token& endToken The end to the ast node tokens
 * @returns SourceRange the range from and to of the ast node
 */
SourceRange ParserUtils::getLocation( const Token& startToken, const Token& endToken ) 
{
    return { 
        startToken.getLocation().start, 
        endToken.getLocation().end,
        endToken.getLocation().fileId
    };
}

/**
 * Gets the source range of the file based on the tokens passed in
 * 
 * @param const Token& token The start and end to the ast node token
 * @returns SourceRange the range from and to of the ast node
 */
SourceRange ParserUtils::getLocation( const Token& token ) 
{
    return token.getLocation();
}

/**
 * Gets operator precedence
 * 
 * @param const std::string& op Operator to check precedence of
 * @returns The precedence number of the operator passed in 
 */
size_t ParserUtils::getPrecedence( TokenSymbol op ) 
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