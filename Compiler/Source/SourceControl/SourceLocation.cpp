#include "SourceControl/SourceLocation.hpp"
#include "Tokens/Token.hpp"

/**
 * Gets the source range of the file based on the tokens passed in
 * 
 * @param const Token& startToken The start to the ast node tokens
 * @param const Token& endToken The end to the ast node tokens
 * @returns SourceRange the range from and to of the ast node
 */
SourceRange SourceRange::getLocation( const Token& startToken, const Token& endToken ) 
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
SourceRange SourceRange::getLocation( const Token& token ) 
{
    return token.getLocation();
}