#include "Parser/ParameterParser.hpp"

std::expected<std::vector<std::unique_ptr<Parameter>>, ErrorVariant> ParameterParser::parseFunctionParameters() 
{   
    auto maybeFrontParens = m_utils.expect( TokenKind::Symbol, TokenSymbol::LParens );
    if ( !maybeFrontParens ) return std::unexpected( maybeFrontParens.error() );

    // Parse parameters: zero or more parameters separated by commas
    std::vector<std::unique_ptr<Parameter>> parameters;

    while ( true ) 
    {
        auto maybeNextToken = m_utils.peek();
        if ( !maybeNextToken ) return std::unexpected( maybeNextToken.error() );
        const Token& next = maybeNextToken.value();

        if ( next.checkMatches( TokenKind::Symbol, TokenSymbol::RParens ) ) 
        {
            // End of parameters list
            auto maybeBackParens = m_utils.expect( TokenKind::Symbol, TokenSymbol::RParens );
            if ( !maybeBackParens ) return std::unexpected( maybeBackParens.error() );
            break;
        }

        auto maybeParameter = parseParameter();
        if ( !maybeParameter ) return std::unexpected( maybeParameter.error() );

        parameters.emplace_back( std::move( maybeParameter.value() ) );

        // After param, expect either ',' or ')'
        auto maybeSeparatorToken = m_utils.peek();
        if ( !maybeSeparatorToken ) return std::unexpected( maybeSeparatorToken.error() );
        const Token& separator = maybeSeparatorToken.value();

        if ( separator.checkMatches( TokenKind::Symbol, TokenSymbol::Comma ) ) 
        {
            auto maybeCommaToken = m_utils.expect( TokenKind::Symbol, TokenSymbol::Comma );
            if ( !maybeCommaToken ) return std::unexpected( maybeCommaToken.error() );
            continue;
        } 
        else if ( separator.checkMatches( TokenKind::Symbol, TokenSymbol::RParens ) ) 
        {
            // will be handled next loop iteration
            continue;
        } 
        else 
        {
            return std::unexpected(
                CompilerError(
                    ErrorSeverity::Error,
                    "Expected ',' or ')' after parameter, got '" + separator.getValue() + "'", 
                    separator.getLocation(),
                    ErrorCategory::Syntax
                )
            );
        }
    }

    return parameters;
}

std::expected<std::unique_ptr<Parameter>, ErrorVariant> ParameterParser::parseParameter() 
{
    auto maybeIdToken = m_utils.consume( TokenKind::Identifier );
    if ( !maybeIdToken ) return std::unexpected( maybeIdToken.error() );
    const Token& idToken = maybeIdToken.value();

    auto maybeColon = m_utils.expect( TokenKind::Symbol, TokenSymbol::Colon );
    if ( !maybeColon ) return std::unexpected( maybeColon.error() );

    auto maybeParsedType = m_typeParser.parseType();
    if ( !maybeParsedType ) return std::unexpected( maybeParsedType.error() );

    auto param = std::make_unique<Parameter>( idToken.getValue(), std::move( maybeParsedType.value() ) );

    auto maybeEndToken = m_utils.peekBack();
    if ( !maybeEndToken ) return std::unexpected( maybeEndToken.error() );
    
    param->location = m_utils.getLocation( idToken, maybeEndToken.value() );

    return param;
}
