#include "Parser/TypeParser.hpp"
#include "Driver/CompilationUnit.hpp"

std::expected<ParsedType*, ErrorVariant> TypeParser::parseType() 
{
    auto frontToken = m_tokenStream.peek();

    if ( frontToken.checkTypeMatches( TokenKind::EndOfFile) ) {
        return std::unexpected( UnexpectedEndOfInputError( frontToken.getLocation() ) );
    }

    if ( frontToken.checkMatches( TokenKind::Keyword, TokenKeyword::Fn ) ) {
        auto maybeFunctionKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Fn );
        if ( !maybeFunctionKeyword ) return std::unexpected( maybeFunctionKeyword.error() );

        auto currentToken = m_tokenStream.peek();
        if ( currentToken.checkTypeMatches( TokenKind::EndOfFile ) ) {
            return std::unexpected( UnexpectedEndOfInputError( currentToken.getLocation() ) );
        }

        std::vector<ParsedType*> params;
        ParsedType* returnType = nullptr;

        if ( currentToken.checkMatches( TokenKind::Symbol, TokenSymbol::LParens ) ) 
        {
            auto maybeParams = parseParameterTypes();
            if ( !maybeParams ) return std::unexpected( maybeParams.error() );

            params = maybeParams.value();

            auto maybeColon = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Colon );
            if ( !maybeColon ) return std::unexpected( maybeColon.error() );

            auto maybeReturnType = parseType();
            if ( !maybeReturnType ) return std::unexpected( maybeReturnType.error() );
            returnType = maybeReturnType.value();
        } 
        else if ( currentToken.checkMatches( TokenKind::Symbol, TokenSymbol::Colon ) ) 
        {
            auto maybeColon = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Colon );
            if ( !maybeColon ) return std::unexpected( maybeColon.error() );

            auto maybeReturnType = parseType();
            if ( !maybeReturnType ) return std::unexpected( maybeReturnType.error() );
            returnType = maybeReturnType.value();
        } 
        else if ( currentToken.checkMatches( TokenKind::Symbol, TokenSymbol::Assign ) )
        {
            returnType = m_compUnit.allocate<ParsedInferredType>();

            returnType->location = currentToken.getLocation();
        } 
        else 
        {
            return std::unexpected( 
                CompilerError(
                    "Unexpected '" + currentToken.getValue() + "' in function type declaration", 
                    ErrorSeverity::Error,
                    currentToken.getLocation(),
                    ErrorCategory::Syntax
                )
            );
        }

        auto typeFunc = m_compUnit.allocate<ParsedFunctionType>( params, returnType );

        typeFunc->location = { frontToken.getLocation().start, typeFunc->returnType->location.end, frontToken.getLocation().fileId };
        
        return typeFunc;
    } 
    else if ( frontToken.checkMatches( TokenKind::Keyword, std::vector<TokenKeyword> 
        { TokenKeyword::Rref, TokenKeyword::Wref, TokenKeyword::Owned, TokenKeyword::Shared, TokenKeyword::Weak } 
    ) ) 
    {
        auto peekToken = m_tokenStream.peek();

        if ( !peekToken.checkTypeMatches( TokenKind::Keyword )) {
            return std::unexpected( UnexpectedTypeError(TokenKind::Keyword, peekToken.getType(), peekToken.getLocation() ));
        }

        auto ownershipKeyword = m_tokenStream.consume().getValue();
        
        OwnershipKind kind;
        
        if ( ownershipKeyword == "rref" ) 
        {
            kind = OwnershipKind::Rref;
        } 
        else if ( ownershipKeyword == "wref" ) 
        {
            kind = OwnershipKind::Wref;
        } 
        else if ( ownershipKeyword == "owned" ) 
        {
            kind = OwnershipKind::Owned;
        } 
        else if ( ownershipKeyword == "shared" ) 
        {
            kind = OwnershipKind::Shared;
        } 
        else if ( ownershipKeyword == "weak" ) 
        {
            kind = OwnershipKind::Weak;
        } 
        else 
        {
            return std::unexpected( 
                CompilerError(
                    "Unexpected keyword " + frontToken.getValue(),
                    ErrorSeverity::Error,
                    frontToken.getLocation(),
                    ErrorCategory::Syntax 
                )
            );
        }
        
        auto maybeReferenceVal = parseType();
        if ( !maybeReferenceVal ) return std::unexpected( maybeReferenceVal.error() );

        auto reference = m_compUnit.allocate<ParsedOwnershipType>( kind, std::move( maybeReferenceVal.value() ) );

        reference->location = { frontToken.getLocation().start, reference->inner->location.end, frontToken.getLocation().fileId };

        return reference;
    } 
    else if ( frontToken.checkMatches( TokenKind::Keyword, TokenKeyword::Infer ) ) 
    {
        auto maybeInfer = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Infer );
        if ( !maybeInfer ) return std::unexpected( maybeInfer.error() );
        
        auto inferType = m_compUnit.allocate<ParsedInferredType>();

        inferType->location = SourceRange::getLocation( frontToken, maybeInfer.value() );

        return inferType;
    }
    else if ( frontToken.checkTypeMatches( TokenKind::Identifier ) ) 
    {
        auto maybeNamedType = parseNamedType();
        if ( !maybeNamedType ) return std::unexpected( maybeNamedType.error() );
        return maybeNamedType.value();
    } 
    else 
    {
        return std::unexpected(
            CompilerError(
                "Parsed type invalid",
                ErrorSeverity::Error,
                frontToken.getLocation(),
                ErrorCategory::Syntax
            ) 
        );
    }   
}

std::expected<ParsedType*, ErrorVariant> TypeParser::parseNamedType() 
{
    auto idToken = m_tokenStream.consume();

    auto identifier = m_compUnit.allocate<Identifier>( idToken.getValue() );

    identifier->location = SourceRange::getLocation(idToken);

    auto namedType = m_compUnit.allocate<ParsedNamedType>( identifier );

    namedType->location = namedType->identifier->location;
        
    return namedType;
}

std::expected<std::vector<ParsedType*>, ErrorVariant> TypeParser::parseParameterTypes() 
{
    auto maybeFrontParens = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::LParens );
    if ( !maybeFrontParens ) return std::unexpected( maybeFrontParens.error() );

    std::vector<ParsedType*> types;

    while( true ) 
    {
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

        if ( current.checkTypeMatches( TokenKind::Symbol ) ) 
        {
            return std::unexpected( 
                CompilerError(
                    "Unexpected '" + current.getValue() + "' in function type declaration parameter list.", 
                    ErrorSeverity::Error,
                    current.getLocation(),
                    ErrorCategory::Syntax
                )
            );
        }

        auto maybeParsedType = parseType();
        if (!maybeParsedType ) return std::unexpected( maybeParsedType.error() );

        types.emplace_back( maybeParsedType.value() );

        auto next = m_tokenStream.peek();

        if ( next.checkTypeMatches( TokenKind::EndOfFile )) {
            return std::unexpected( UnexpectedEndOfInputError( current.getLocation() ) );
        }

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
                        "Unexpected '" + next.getValue() + "' in function type declaration parameter list.", 
                        ErrorSeverity::Error,
                        next.getLocation(),
                        ErrorCategory::Syntax
                    )
                );
            } 
            
            auto maybeComma = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Comma );
            if ( !maybeComma ) return std::unexpected( maybeComma.error() );
        }
    }

    return types;
}