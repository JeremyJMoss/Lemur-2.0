#include "Parser/TypeParser.hpp"

std::expected<std::unique_ptr<ParsedType>, ErrorVariant> TypeParser::parseType() 
{
    auto maybeFrontToken = m_utils.peek();
    if ( !maybeFrontToken ) return std::unexpected( maybeFrontToken.error() );
    const Token& front = maybeFrontToken.value();

    if ( front.checkMatches( TokenKind::Keyword, TokenKeyword::Fn ) ) {
        auto maybeFunctionKeyword = m_utils.expect( TokenKind::Keyword, TokenKeyword::Fn );
        if ( !maybeFunctionKeyword ) return std::unexpected( maybeFunctionKeyword.error() );

        auto maybeCurrentToken = m_utils.peek();
        if ( !maybeCurrentToken ) return std::unexpected( maybeCurrentToken.error() );
        const Token& current = maybeCurrentToken.value();

        std::vector<std::unique_ptr<ParsedType>> params;
        std::unique_ptr<ParsedType> returnType;

        if ( current.checkMatches( TokenKind::Symbol, TokenSymbol::LParens ) ) 
        {
            auto maybeParams = parseParameterTypes();
            if ( !maybeParams ) return std::unexpected( maybeParams.error() );

            params = std::move( maybeParams.value() );

            auto maybeColon = m_utils.expect( TokenKind::Symbol, TokenSymbol::Colon );
            if ( !maybeColon ) return std::unexpected( maybeColon.error() );

            auto maybeReturnType = parseType();
            if ( !maybeReturnType ) return std::unexpected( maybeReturnType.error() );
            returnType = std::move( maybeReturnType.value() );
        } 
        else if ( current.checkMatches( TokenKind::Symbol, TokenSymbol::Colon ) ) 
        {
            auto maybeColon = m_utils.expect( TokenKind::Symbol, TokenSymbol::Colon );
            if ( !maybeColon ) return std::unexpected( maybeColon.error() );

            auto maybeReturnType = parseType();
            if ( !maybeReturnType ) return std::unexpected( maybeReturnType.error() );
            returnType = std::move( maybeReturnType.value() );
        } 
        else if ( current.checkMatches( TokenKind::Symbol, TokenSymbol::Assign ) )
        {
            returnType = std::make_unique<ParsedInferredType>();

            auto maybeEndToken = m_utils.peekBack();
            if ( !maybeEndToken ) return std::unexpected( maybeEndToken.error() );

            returnType->location = m_utils.getLocation( current, maybeEndToken.value() );
        } 
        else 
        {
            return std::unexpected( 
                CompilerError(
                    ErrorSeverity::Error,
                    "Unexpected '" + current.getValue() + "' in function type declaration", 
                    current.getLocation(),
                    ErrorCategory::Syntax
                )
            );
        }

        auto typeFunc = std::make_unique<ParsedFunctionType>( std::move( params ), std::move( returnType ) );

        auto maybeEndToken = m_utils.peekBack();
        if ( !maybeEndToken ) return std::unexpected( maybeEndToken.error() );

        typeFunc->location = m_utils.getLocation( front, maybeEndToken.value() );
        
        return typeFunc;
    } 
    else if ( front.checkMatches( TokenKind::Keyword, std::vector<TokenKeyword> 
        { TokenKeyword::View, TokenKeyword::Mut, TokenKeyword::Own, TokenKeyword::Share, TokenKeyword::Weak, TokenKeyword::Atomic } 
    ) ) 
    {
        auto maybeOwnershipKeyword = m_utils.consume( TokenKind::Keyword );
        if ( !maybeOwnershipKeyword ) return std::unexpected( maybeOwnershipKeyword.error() );

        const std::string& ownershipKeyword = maybeOwnershipKeyword.value().getValue();
        
        OwnershipKind kind;
        
        if ( ownershipKeyword == "view" ) 
        {
            kind = OwnershipKind::View;
        } 
        else if ( ownershipKeyword == "mut" ) 
        {
            kind = OwnershipKind::Mut;
        } 
        else if ( ownershipKeyword == "own" ) 
        {
            kind = OwnershipKind::Own;
        } 
        else if ( ownershipKeyword == "share" ) 
        {
            kind = OwnershipKind::Share;
        } 
        else if ( ownershipKeyword == "atomic" ) 
        {
            kind = OwnershipKind::Atomic;
        } 
        else if ( ownershipKeyword == "weak" ) 
        {
            kind = OwnershipKind::Weak;
        } 
        else 
        {
            return std::unexpected( 
                CompilerError( 
                    ErrorSeverity::Error,
                    "Unexpected keyword " + front.getValue(), front.getLocation(), 
                    ErrorCategory::Syntax 
                )
            );
        }
        
        auto maybeReferenceVal = parseType();
        if ( !maybeReferenceVal ) return std::unexpected( maybeReferenceVal.error() );

        auto reference = std::make_unique<ParsedOwnershipType>( kind, std::move( maybeReferenceVal.value() ) );

        auto maybeEndToken = m_utils.peekBack();
        if ( !maybeEndToken ) return std::unexpected( maybeEndToken.error() );

        reference->location = m_utils.getLocation( front, maybeEndToken.value() );

        return reference;
    } 
    else if ( front.checkMatches( TokenKind::Keyword, TokenKeyword::Infer ) ) 
    {
        auto maybeInfer = m_utils.expect( TokenKind::Keyword, TokenKeyword::Infer );
        if ( !maybeInfer ) return std::unexpected( maybeInfer.error() );
        
        auto inferType = std::make_unique<ParsedInferredType>();

        auto maybeEndToken = m_utils.peekBack();
        if ( !maybeEndToken ) return std::unexpected( maybeEndToken.error() );

        inferType->location = m_utils.getLocation( front, maybeEndToken.value() );

        return inferType;
    } 
    else if ( front.checkTypeMatches( TokenKind::Identifier ) ) 
    {
        auto maybeGenericType = parseGenericType();
        if ( !maybeGenericType ) return std::unexpected( maybeGenericType.error() );
        auto genericType = std::move( maybeGenericType.value() );

        auto maybeEndToken = m_utils.peekBack();
        if ( !maybeEndToken ) return std::unexpected( maybeEndToken.error() );

        genericType->location = m_utils.getLocation( front, maybeEndToken.value() );

        return genericType;   
    } 
    else 
    {
        return std::unexpected(
            CompilerError(
                ErrorSeverity::Error,
                "Parsed type invalid",
                front.getLocation(),
                ErrorCategory::Syntax
            ) 
        );
    }   
}

std::expected<std::unique_ptr<ParsedType>, ErrorVariant> TypeParser::parseGenericType() 
{
    auto maybeIdToken = m_utils.consume( TokenKind::Identifier );
    if ( !maybeIdToken ) return std::unexpected( maybeIdToken.error() );
    const Token& idToken = maybeIdToken.value();

    auto identifier = std::make_unique<Identifier>( idToken.getValue() );

    auto maybeCurrentToken = m_utils.peek();
    if ( !maybeCurrentToken ) return std::unexpected( maybeCurrentToken.error() );
    const Token& current = maybeCurrentToken.value();

    if ( !current.checkValueMatches( TokenSymbol::Less ) ) 
    {
        return std::make_unique<ParsedNamedType>( std::move( identifier ) );
    }

    auto maybeCustomTypeArgs = parseGenericTypeArgs();
    if ( !maybeCustomTypeArgs ) return std::unexpected( maybeCustomTypeArgs.error() );

    return std::make_unique<ParsedGenericType>( std::move( identifier ) , std::move( maybeCustomTypeArgs.value() ) );
}

std::expected<std::vector<std::unique_ptr<ParsedType>>, ErrorVariant> TypeParser::parseGenericTypeArgs() 
{
    auto maybeAngleBracket = m_utils.expect( TokenKind::Symbol, TokenSymbol::Less );
    if ( !maybeAngleBracket ) return std::unexpected( maybeAngleBracket.error() );

    std::vector<std::unique_ptr<ParsedType>> arguments;

    while( true ) 
    {
        auto maybeCurrentToken = m_utils.peek();
        if ( !maybeCurrentToken ) return std::unexpected( maybeCurrentToken.error() );
        const Token& current = maybeCurrentToken.value();

        if ( current.checkMatches( TokenKind::Symbol, TokenSymbol::Greater ) ) 
        {
            auto maybeReverseAngle = m_utils.expect( TokenKind::Symbol, TokenSymbol::Greater );
            if ( !maybeReverseAngle ) return std::unexpected( maybeReverseAngle.error() );

            return arguments;
        } 
        
        if ( current.checkTypeMatches( TokenKind::Identifier ) ) 
        {
            auto maybeGenericType = parseGenericType();
            if ( !maybeGenericType ) return std::unexpected( maybeGenericType.error() );
            auto genericType = std::move( maybeGenericType.value() );

            auto maybeEndToken = m_utils.peekBack();
            if ( !maybeEndToken ) return std::unexpected( maybeEndToken.error() );

            genericType->location = m_utils.getLocation( current, maybeEndToken.value() );

            arguments.emplace_back( std::move( genericType ) );
        } 
        else 
        {
            return std::unexpected( 
                CompilerError(
                    ErrorSeverity::Error,
                    "Expected identifier or primitive type for '" + current.getValue() + "'", 
                    current.getLocation(),
                    ErrorCategory::Syntax
                ) 
            );
        }

        auto maybeAfterToken = m_utils.peek();
        if ( !maybeAfterToken ) return std::unexpected( maybeAfterToken.error() );
        const Token& after = maybeAfterToken.value();

        if ( after.checkMatches( TokenKind::Symbol, TokenSymbol::Comma ) ) 
        {
            auto maybeComma = m_utils.expect( TokenKind::Symbol, TokenSymbol::Comma );
            if ( !maybeComma ) return std::unexpected( maybeComma.error() );
            continue;
        } 
        else if ( after.checkMatches( TokenKind::Symbol, TokenSymbol::Greater ) ) 
        {
            continue;
        }

        return std::unexpected( 
            CompilerError(
                ErrorSeverity::Error,
                "Expected ',' or '>' got '" + after.getValue() + "'", 
                after.getLocation(),
                ErrorCategory::Syntax
            ) 
        );
    }
}

std::expected<std::vector<std::unique_ptr<ParsedType>>, ErrorVariant> TypeParser::parseParameterTypes() 
{
    auto maybeFrontParens = m_utils.expect( TokenKind::Symbol, TokenSymbol::LParens );
    if ( !maybeFrontParens ) return std::unexpected( maybeFrontParens.error() );

    std::vector<std::unique_ptr<ParsedType>> types;

    while( true ) 
    {
        auto maybeCurrentToken = m_utils.peek();
        if ( !maybeCurrentToken ) return std::unexpected( maybeCurrentToken.error() );
        const Token& current = maybeCurrentToken.value();

        if ( current.checkMatches( TokenKind::Symbol, TokenSymbol::RParens ) ) 
        {
            auto maybeClosingParens = m_utils.expect( TokenKind::Symbol, TokenSymbol::RParens );
            if ( !maybeClosingParens ) return std::unexpected( maybeClosingParens.error() );
            break;
        }

        if ( current.checkTypeMatches( TokenKind::Symbol ) ) 
        {
            return std::unexpected( 
                CompilerError(
                    ErrorSeverity::Error,
                    "Unexpected '" + current.getValue() + "' in function type declaration parameter list.", 
                    current.getLocation(),
                    ErrorCategory::Syntax
                )
            );
        }

        auto maybeParsedType = parseType();
        if (!maybeParsedType ) return std::unexpected( maybeParsedType.error() );

        types.emplace_back( std::move( maybeParsedType.value() ) );

        auto maybeNextToken = m_utils.peek();
        if ( !maybeNextToken ) return std::unexpected( maybeNextToken.error() );
        const Token& next = maybeNextToken.value();

        if ( next.checkTypeMatches( TokenKind::Symbol ) ) 
        {
            if ( next.checkValueMatches( TokenSymbol::RParens ) ) 
            {
                continue;
            } 
            else if ( !next.checkValueMatches( TokenSymbol::Comma ) ) 
            {
                return std::unexpected( 
                    CompilerError(
                        ErrorSeverity::Error,
                        "Unexpected '" + next.getValue() + "' in function type declaration parameter list.", 
                        next.getLocation(),
                        ErrorCategory::Syntax
                    )
                );
            } 
            
            auto maybeComma = m_utils.expect( TokenKind::Symbol, TokenSymbol::Comma );
            if ( !maybeComma ) return std::unexpected( maybeComma.error() );
        }
    }

    return types;
}