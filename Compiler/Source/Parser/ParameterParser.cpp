#include "Parser/ParameterParser.hpp"
#include "Tokens/TokenStream.hpp"
#include "Driver/CompilationUnit.hpp"
#include "Parser/TypeParser.hpp"
#include "AST/Identifier.hpp"
#include "AST/ParsedType.hpp"
#include "AST/Parameter.hpp"

std::expected<std::vector<Parameter*>, ErrorVariant> ParameterParser::parseFunctionParameters() {    
    auto maybeFrontParens = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::LParens );
    if ( !maybeFrontParens ) return std::unexpected( maybeFrontParens.error() );

    // Parse parameters: zero or more parameters separated by commas
    std::vector<Parameter*> parameters;

    while ( true ) 
    {
        const Token& next = m_tokenStream.peek();

        if ( next.checkTypeMatches( TokenKind::EndOfFile )) {
            return std::unexpected( UnexpectedEndOfInputError( next.getLocation() ) );
        }

        if ( next.checkMatches( TokenKind::Symbol, TokenSymbol::RParens ) ) 
        {
            // End of parameters list
            auto maybeBackParens = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::RParens );
            if ( !maybeBackParens ) return std::unexpected( maybeBackParens.error() );
            break;
        }

        auto maybeParameter = parseParameter();
        if ( !maybeParameter ) return std::unexpected( maybeParameter.error() );

        parameters.emplace_back( maybeParameter.value() );

        // After param, expect either ',' or ')'
        auto seperator = m_tokenStream.peek();

        if ( seperator.checkMatches( TokenKind::Symbol, TokenSymbol::Comma ) ) 
        {
            auto maybeCommaToken = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Comma );
            if ( !maybeCommaToken ) return std::unexpected( maybeCommaToken.error() );
            continue;
        } 
        else if ( seperator.checkMatches( TokenKind::Symbol, TokenSymbol::RParens ) ) 
        {
            // will be handled next loop iteration
            continue;
        } 
        else 
        {
            return std::unexpected(
                CompilerError(
                    "Expected ',' or ')' after parameter, got '" + seperator.getValue() + "'", 
                    ErrorSeverity::Error,
                    seperator.getLocation(),
                    ErrorCategory::Syntax
                )
            );
        }
    }

    return parameters;
}

std::expected<Parameter*, ErrorVariant> ParameterParser::parseParameter() 
{
    const Token& front = m_tokenStream.peek();

    if ( !front.checkTypeMatches( TokenKind::Identifier )) {
        return std::unexpected( UnexpectedTypeError( TokenKind::Identifier, front.getType(), front.getLocation() ) );
    }

    const Token& idToken = m_tokenStream.consume();

    auto identifier = m_compUnit.allocate<Identifier>(idToken.getValue());

    identifier->location = SourceRange::getLocation(idToken);

    auto maybeColon = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Colon );
    if ( !maybeColon ) return std::unexpected( maybeColon.error() );

    auto maybeParsedType = m_typeParser.parseType();
    if ( !maybeParsedType ) return std::unexpected( maybeParsedType.error() );

    auto param = m_compUnit.allocate<Parameter>( identifier, maybeParsedType.value() );
    
    param->location = { front.getLocation().start, param->paramType->location.end, front.getLocation().fileId };

    return param;
}
