#include "Parser/StatementParser.hpp"
#include "Parser/Parser.hpp"
#include "Utils/Logger.hpp"
#include "Errors/Errors.hpp"
#include "Errors/ErrorReporter.hpp"
#include "AST/ASTPrinter.hpp"
#include "AST/FunctionDeclaration.hpp"
#include "AST/Block.hpp"
#include "AST/Identifier.hpp"
#include "AST/VariableDeclaration.hpp"
#include "AST/Return.hpp"
#include "AST/IfConditional.hpp"
#include "AST/ForLoop.hpp"
#include "AST/ModuleDeclaration.hpp"
#include "AST/Import.hpp"
#include "AST/ImportedSymbol.hpp"
#include "AST/QualifiedName.hpp"
#include "Driver/CompilationUnit.hpp"

#include <expected>
#include <variant>

std::expected<FunctionDeclaration*, Diagnostic> StatementParser::parseFunctionDeclaration( const bool isEntry, DeclarationVisibility visibility ) 
{
    Logger::debug( 
        "Parsing function declaration",
        std::to_array<Attribute>({ 
            { "Entry Function", ( isEntry ? "true" : "false" ) } 
        })
    );

    const Token& front = m_tokenStream.peek();

    if ( front.checkTypeMatches( TokenKind::EndOfFile ) ) {
        return std::unexpected(
            UnexpectedEndOfInputDiagnostic(
                front.getLocation()
            )
        );
    }

    if ( isEntry ) 
    {
        auto maybeEntryKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Entry );
        if ( !maybeEntryKeyword )
        {
            // Report entry keyword missing from entry function
            m_errReporter.report(
                Diagnostic(
                    "Entry keyword missing from entry function",
                    ErrorCategory::Syntax,
                    ErrorSeverity::Warning,
                    front.getLocation()
                )
            );
        }
    }

    auto maybeFunctionKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Fn );
    if ( !maybeFunctionKeyword ) 
    {
        m_errReporter.report( maybeFunctionKeyword.error() );
    }

    auto peekedToken = m_tokenStream.peek();

    if ( !peekedToken.checkTypeMatches( TokenKind::Identifier ) ) 
    {
        return std::unexpected(
            UnexpectedTypeDiagnostic( 
                TokenKind::Identifier, 
                peekedToken.getType(), 
                peekedToken.getLocation()
            )
        );
    }

    auto idToken = m_tokenStream.consume();

    Logger::trace(
        "Function name parsed", 
        std::to_array<Attribute>({
            { "Identifier", std::format("'{}'", idToken.getValue() ) }
        })
    );

    auto identifier = m_compUnit.allocate<Identifier>( idToken.getValue() );

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
            { "Type", std::format( "'{}'", ASTPrinter::getParsedType( maybeReturnType.value()->kind ) ) }
        })
    );

    auto maybeBody = parseBlock();
    if ( !maybeBody ) return std::unexpected( maybeBody.error() );
    auto block = maybeBody.value();

    Logger::trace(
        "Function body parsed"
    );

    auto funDec = m_compUnit.allocate<FunctionDeclaration>(
        identifier, 
        maybeReturnType.value(), 
        parameters, 
        block,
        true,
        isEntry
    );

    funDec->location = { front.getLocation().start, funDec->body->location.end, front.getLocation().fileId};

    funDec->visibility = visibility;

    Logger::debug(
        "Function declaration parsed successfully",
        std::to_array<Attribute>({
            { "Name", std::format( "'{}'", idToken.getValue() ) }
        })
    );

    return funDec;
}

std::expected<Block*, Diagnostic> StatementParser::parseBlock() 
{
    const Token& front = m_tokenStream.peek();

    if (front.checkTypeMatches( TokenKind::EndOfFile )){
        return std::unexpected(
            UnexpectedEndOfInputDiagnostic(
                front.getLocation()
            )
        );
    }

    Logger::debug(
        "Parsing block", 
        std::to_array<Attribute>({ 
            { "StartToken", front.getLocation().toString() } 
        })
    );

    auto maybeFrontBrace = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::LBrace );
    if ( !maybeFrontBrace ) return std::unexpected( maybeFrontBrace.error() );

    std::vector<Statement*> body;

    Logger::trace( "Parsing statements inside block" );

    auto current = m_tokenStream.peek();

    while ( true ) 
    {
        if ( current.checkTypeMatches( TokenKind::EndOfFile ) ) {
            return std::unexpected(
                UnexpectedEndOfInputDiagnostic(
                    current.getLocation()
                ) 
            );
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
            m_errReporter.report( maybeStatement.error() );

            Logger::trace( "Recovering from error inside block" );

            m_tokenStream.recoverFromError();

            current = m_tokenStream.peek();
            continue;
        }

        body.emplace_back( maybeStatement.value() );

        current = m_tokenStream.peek();
    }

    auto block = m_compUnit.allocate<Block>( body );

    block->location = SourceRange::getLocation( front, current );

    Logger::debug( 
        "Parsing statements inside block successful",
        std::to_array<Attribute>({
            { "StatementCount", std::to_string( block->statements.size() ) }
        })
    );

    return block;
}

std::expected<VariableDeclaration*, Diagnostic> StatementParser::parseVariableDeclaration( const bool locked, DeclarationVisibility visibility ) 
{
    Logger::debug(
        "Parsing variable declaration", 
        std::to_array<Attribute>({ 
            { "Locked", ( locked ? "true" : "false" ) } 
        })
    );

    const Token& front = m_tokenStream.peek();

    if ( front.checkTypeMatches( TokenKind::EndOfFile ) ) {
        return std::unexpected( 
            UnexpectedEndOfInputDiagnostic( 
                front.getLocation() 
            ) 
        );
    }

    if ( locked ) 
    {
        auto maybeLocked = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Lock );
        if ( !maybeLocked )
        {
            // Report locked variable missing lock keyword
            return std::unexpected(
                Diagnostic(
                    "Locked variable missing lock keyword",
                    ErrorCategory::Syntax,
                    ErrorSeverity::Error,
                    front.getLocation()
                )
            );
        }
    }

    const Token& peekedToken = m_tokenStream.peek();

    if (!peekedToken.checkTypeMatches(TokenKind::Identifier)) {
        return std::unexpected(
            UnexpectedTypeDiagnostic( 
                TokenKind::Identifier, 
                peekedToken.getType(), 
                peekedToken.getLocation()
            )
        );
    }

    const Token& idToken = m_tokenStream.consume();

    Logger::trace(
        "Consumed variable identifier", 
        std::to_array<Attribute>({ 
            { "Identifier", std::format( "'{}'", idToken.getValue() ) } 
        })
    );

    auto identifier = m_compUnit.allocate<Identifier>( idToken.getValue() );
    identifier->location = SourceRange::getLocation( front, idToken );

    auto maybeColon = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Colon );
    if ( !maybeColon ) return std::unexpected( maybeColon.error() );

    auto maybeParsedType = m_typeParser.parseType();
    if ( !maybeParsedType )
    {
        return std::unexpected(
            Diagnostic(
                "Could not parse type.",
                ErrorCategory::Syntax,
                ErrorSeverity::Error,
                maybeColon.value().get().getLocation()
            )
        );
    }

    auto varType = maybeParsedType.value();

    Logger::trace(
        "Parsed variable type", 
        std::to_array<Attribute>({ 
            { "Type", std::format("'{}'", ASTPrinter::getParsedType( varType->kind ) ) } 
        })
    );

    const Token& current = m_tokenStream.peek();

    if (current.checkTypeMatches(TokenKind::EndOfFile)) {
        return std::unexpected(
            UnexpectedEndOfInputDiagnostic( 
                current.getLocation() 
            )
        );
    }

    Expression* initialiser = nullptr;

    if ( current.checkMatches( TokenKind::Symbol, TokenSymbol::Assign ) )
    {
        Logger::trace(
            "Detected assignment in variable declaration", 
            std::to_array<Attribute>({ 
                { "Identifier", std::format( "'{}'", idToken.getValue() ) } 
            })
        );

        auto maybeInitialiser = m_exprParser->parseInitialiser();

        if ( !maybeInitialiser ) 
        {
            return std::unexpected( maybeInitialiser.error() );
        }
        else 
        {
            initialiser = maybeInitialiser.value();
        }
    }

    auto endLocation = initialiser != nullptr ? initialiser->location.end : varType->location.end;

    auto decl = m_compUnit.allocate<VariableDeclaration>( 
        identifier, 
        varType, 
        locked, 
        initialiser 
    );

    decl->location = {front.getLocation().start, endLocation, front.getLocation().fileId };

    decl->visibility = visibility;

    Logger::debug(
        "Completed variable declaration", 
        std::to_array<Attribute>({ 
            { "Identifier", std::format( "'{}'", decl->identifier->name ) },
            { "HasInitialiser", ( initialiser ? "true" : "false" ) },
            { "Exported", ( visibility == DeclarationVisibility::Public ? "true" : "false" ) }
        })
    );

    return decl;
}

std::expected<Return*, Diagnostic> StatementParser::parseReturn() 
{
    Logger::debug( "Parsing return statement" );

    const Token& front = m_tokenStream.peek();

    if ( front.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( 
            UnexpectedEndOfInputDiagnostic( 
                front.getLocation() 
            ) 
        );
    }

    auto maybeReturnKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Return );
    if ( !maybeReturnKeyword ) return std::unexpected( maybeReturnKeyword.error() );

    const Token& next = m_tokenStream.peek();

    if ( next.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( 
            UnexpectedEndOfInputDiagnostic( 
                next.getLocation() 
            ) 
        );
    }

    Expression* value = nullptr;

    Logger::trace( "Checking for return value expression" );

    // bare  `return ;`
    if ( !next.checkMatches( TokenKind::Symbol, TokenSymbol::SemiColon ) ) {
        auto maybeValue = m_exprParser->parseExpression();
        if ( !maybeValue ) 
        {
            return std::unexpected( 
                Diagnostic(
                    "Unable to parse return statement. Malformed expression statement after return keyword",
                    ErrorCategory::Syntax,
                    ErrorSeverity::Error,
                    next.getLocation()
                )
            );
        }
        value = maybeValue.value();
    }

    bool hasReturn = value != nullptr;

    Logger::trace( "Parsed return value expression" );

    auto ret = m_compUnit.allocate<Return>( value );

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

std::expected<IfConditional*, Diagnostic> StatementParser::parseIfConditional( bool justElse ) 
{
    std::string stmtType = justElse ? "else" : "if";
    Logger::debug( "Parsing " + stmtType + " statement" );

    const Token& front = m_tokenStream.peek();

    if ( !front.checkTypeMatches( TokenKind::Keyword )) {
        return std::unexpected( 
            UnexpectedTypeDiagnostic( 
                TokenKind::Keyword, 
                front.getType(), 
                front.getLocation() 
            ) 
        );
    }

    m_tokenStream.consume();

    Expression* condition = nullptr;

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

        condition = maybeCondition.value();

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

    IfConditional* elseStatement = nullptr;

    auto next = m_tokenStream.peek();

    if ( next.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( 
            UnexpectedEndOfInputDiagnostic( 
                next.getLocation() 
            )
        );
    }
    
    if ( next.checkMatches( TokenKind::Keyword, TokenKeyword::Else ) ) 
    {
        Logger::trace( "Detected 'else', checking for 'if' to parse else-if or else body" );

        next = m_tokenStream.peek( 1 );

        if ( next.checkTypeMatches( TokenKind::EndOfFile )) {
            return std::unexpected( 
                UnexpectedEndOfInputDiagnostic( 
                    next.getLocation() 
                ) 
            );
        }

        if ( next.checkMatches( TokenKind::Keyword, TokenKeyword::If ) ) 
        {
            Logger::trace( "Detected else-if branch, calling parseIfConditional recursively" );

            auto maybeIf = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::If );
            if ( !maybeIf ) return std::unexpected( maybeIf.error() );

            auto maybeElseStatement = parseIfConditional();
            if ( !maybeElseStatement ) return std::unexpected( maybeElseStatement.error() );
            elseStatement = maybeElseStatement.value();
        } 
        else {
            Logger::trace( "Detected 'else' branch, parsing recursively" );

            auto maybeElseStatement = parseIfConditional( true );
            if ( !maybeElseStatement ) return std::unexpected( maybeElseStatement.error() );
            elseStatement = maybeElseStatement.value();
        }
    }

    auto endLocation = elseStatement == nullptr ? maybeBody.value()->location.end : elseStatement->location.end;

    auto ifStmt = m_compUnit.allocate<IfConditional>( 
        condition, 
        elseStatement, 
        maybeBody.value() 
    );

    ifStmt->location = { front.getLocation().start, endLocation, front.getLocation().fileId };

    Logger::debug( "Successfully parsed " + stmtType + " statement" );

    return ifStmt;
}

std::expected<ForLoop*, Diagnostic> StatementParser::parseForLoop() 
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

    Expression* step = nullptr;
    Expression* where = nullptr;

    const Token& current = m_tokenStream.peek();

    if ( current.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( 
            UnexpectedEndOfInputDiagnostic( 
                current.getLocation() 
            ) 
        );
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
                Diagnostic(
                    "Malformed or missing step statement inside loop parameters",
                    ErrorCategory::Syntax,
                    ErrorSeverity::Error,
                    maybeStepKeyword.value().get().getLocation()
                )
            );
        }
        else
        {
            step = maybeStep.value();
        }

        const Token& next = m_tokenStream.peek();

        if ( next.checkTypeMatches( TokenKind::EndOfFile )) {
            return std::unexpected( 
                UnexpectedEndOfInputDiagnostic( 
                    next.getLocation() 
                ) 
            );
        }

        if ( next.checkMatches( TokenKind::Keyword, TokenKeyword::Where ) ) 
        {
            Logger::trace( "Parsing optional 'where' clause in for loop" );

            auto maybeWhereKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Where );
            if ( !maybeWhereKeyword ) return std::unexpected( maybeWhereKeyword.error() );

            auto maybeWhere = m_exprParser->parseExpression();
            if ( !maybeWhere )
            {
                m_errReporter.report(
                    Diagnostic(
                        "Malformed or missing where statement inside loop parameters",
                        ErrorCategory::Syntax,
                        ErrorSeverity::Error,
                        maybeWhereKeyword.value().get().getLocation()
                    )
                );
            }
            else
            {
                where = maybeWhere.value();
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
                Diagnostic(
                    "Malformed or missing where statement inside loop parameters",
                    ErrorCategory::Syntax,
                    ErrorSeverity::Error,
                    maybeWhereKeyword.value().get().getLocation()
                )
            );
        }
        else
        {
            where = maybeWhere.value();
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

    auto forLoop = m_compUnit.allocate<ForLoop>( 
        maybeLoopVar.value(), 
        maybeIterable.value(), 
        step, 
        where, 
        maybeBody.value() 
    );

    forLoop->location = { maybeForKeyword.value().get().getLocation().start, forLoop->body->location.end, forLoop->body->location.fileId };

    Logger::debug( "Successfully parsed for loop statement" );

    return forLoop;
}

std::expected<ModuleDeclaration*, Diagnostic> StatementParser::parseModuleDeclaration()
{
    Logger::debug( "Parsing module declaration" );

    const Token& front = m_tokenStream.peek();

    if ( front.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( 
            UnexpectedEndOfInputDiagnostic( 
                front.getLocation() 
            ) 
        );
    }

    auto maybeModuleKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Module );
    if ( !maybeModuleKeyword ) return std::unexpected( maybeModuleKeyword.error() );

    auto maybeBuiltModuleName = parseModuleName();

    if ( !maybeBuiltModuleName ) {
        return std::unexpected( maybeBuiltModuleName.error() );
    }

    if ( m_compUnit.getModuleName() != maybeBuiltModuleName.value() ) {
        throw InternalCompilerError( "Module declaration differs from header scan.\nPlease report this bug." );
    }

    QualifiedName* moduleName = m_compUnit.allocate<QualifiedName>( std::string( m_compUnit.getModuleName() ) );

    ModuleDeclaration* modDec = m_compUnit.allocate<ModuleDeclaration>( moduleName );

    modDec->location = { front.getLocation().start, modDec->name->location.end, front.getLocation().fileId };

    Logger::debug( "Successfully parsed module declaration statement" );

    return modDec;
}

std::expected<std::string, Diagnostic> StatementParser::parseModuleName()
{
    const Token& front = m_tokenStream.peek();

    if ( front.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( 
            UnexpectedEndOfInputDiagnostic( 
                front.getLocation() 
            ) 
        );
    }

    TokenKind nextTokenTypeExpected = TokenKind::Identifier;

    std::string builtModuleName = "";

    while (true) 
    {
        const Token& next = m_tokenStream.peek();

        if ( next.checkTypeMatches( nextTokenTypeExpected ) ) 
        {
            if ( nextTokenTypeExpected == TokenKind::Identifier ) 
            {
                builtModuleName += next.getValue();
                m_tokenStream.consume();
                nextTokenTypeExpected = TokenKind::Symbol;
                continue;
            }

            if ( nextTokenTypeExpected == TokenKind::Symbol && next.checkValueMatches( TokenSymbol::Dot ) ) 
            {
                builtModuleName += next.getValue();
                m_tokenStream.consume();
                nextTokenTypeExpected = TokenKind::Identifier;
                continue;
            }

            if ( nextTokenTypeExpected == TokenKind::Symbol && next.checkValueMatches( TokenSymbol::SemiColon ) ) {
                break;
            }

            if ( nextTokenTypeExpected == TokenKind::Symbol && next.checkMatches( TokenKind::Keyword, TokenKeyword::As )) {
                break;
            }

            if ( nextTokenTypeExpected == TokenKind::Symbol) {
                return std::unexpected( 
                    Diagnostic(
                        "Malformed module declaration statement. Missing terminating node.",
                        ErrorCategory::Syntax,
                        ErrorSeverity::Error,
                        next.getLocation()
                    ) 
                );
            } else {
                return std::unexpected(
                    Diagnostic(
                        std::format( 
                            "Malformed module declaration statement. Expected '{}' got '{}'",
                            toString( TokenKind::Identifier ),
                            toString( next.getType() )
                        ),
                        ErrorCategory::Syntax,
                        ErrorSeverity::Error,
                        next.getLocation()
                    )
                );
            }
        }
    }

    if ( builtModuleName.empty() ) {
        return std::unexpected(
            Diagnostic(
                "Empty module declaration statement",
                ErrorCategory::Syntax,
                ErrorSeverity::Error,
                front.getLocation()
            )
        );
    }

    return builtModuleName;
}

std::expected<Import*, Diagnostic> StatementParser::parseImport()
{
    Logger::debug( "Parsing import statement" );

    const Token& front = m_tokenStream.peek();

    if ( front.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( 
            UnexpectedEndOfInputDiagnostic( 
                front.getLocation() 
            ) 
        );
    }

    auto maybeImportKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::Import );
    if ( !maybeImportKeyword ) return std::unexpected( maybeImportKeyword.error() );

    const Token& next = m_tokenStream.peek();

    std::string moduleName = "";
    std::vector<ImportedSymbol*> importedSymbols;
    std::optional<Identifier*> alias;

    if ( next.checkTypeMatches( TokenKind::EndOfFile ) ) {
        return std::unexpected( 
            UnexpectedEndOfInputDiagnostic( 
                front.getLocation() 
            ) 
        );
    } else if ( next.checkTypeMatches( TokenKind::Identifier ) ) {
        auto maybeBuiltModuleName = parseModuleName();

        if ( !maybeBuiltModuleName ) {
            return std::unexpected( maybeBuiltModuleName.error() );
        }

        moduleName = maybeBuiltModuleName.value();

        const Token& current = m_tokenStream.peek();

        if ( current.checkMatches( TokenKind::Keyword, TokenKeyword::As ))
        {
            auto maybeAsKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::As );
            if ( !maybeAsKeyword ) return std::unexpected( maybeAsKeyword.error() );

            const Token& aliasToken = m_tokenStream.peek();

            if (!aliasToken.checkTypeMatches(TokenKind::Identifier)) {
                return std::unexpected(
                    UnexpectedTypeDiagnostic( 
                        TokenKind::Identifier, 
                        aliasToken.getType(), 
                        aliasToken.getLocation()
                    )
                );
            }

            m_tokenStream.consume();

            Identifier* aliasIdentifier = m_compUnit.allocate<Identifier>( aliasToken.getValue() );

            aliasIdentifier->location = SourceRange::getLocation(aliasToken);

            alias = aliasIdentifier;
        }
    } else if ( next.checkMatches( TokenKind::Symbol, TokenSymbol::LBrace ) ) {
        auto maybeImportedSymbols = parseImportedSymbols();

        if ( !maybeImportedSymbols ) return std::unexpected( maybeImportedSymbols.error() );

        importedSymbols = std::move( maybeImportedSymbols.value() );

        auto maybeFromKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::From );
        if ( !maybeFromKeyword ) return std::unexpected( maybeFromKeyword.error() );

        auto maybeBuiltModuleName = parseModuleName();

        if ( !maybeBuiltModuleName ) {
            return std::unexpected( maybeBuiltModuleName.error() );
        }

        moduleName = maybeBuiltModuleName.value();
    } else {
        return std::unexpected(
            Diagnostic(
                "Expected module name or import symbol list",
                ErrorCategory::Syntax,
                ErrorSeverity::Error,
                next.getLocation()
            )
        );
    }

    QualifiedName* qualName = m_compUnit.allocate<QualifiedName>( moduleName );

    const Token& endingToken = m_tokenStream.peek();

    Import* importStmt = m_compUnit.allocate<Import>( qualName, alias, importedSymbols );

    importStmt->location = SourceRange::getLocation( front, endingToken );

    return importStmt;
}

std::expected<std::vector<ImportedSymbol*>, Diagnostic> StatementParser::parseImportedSymbols()
{
    Logger::debug( "Parsing imported symbols" );

    const Token& front = m_tokenStream.peek();

    if ( front.checkTypeMatches( TokenKind::EndOfFile )) {
        return std::unexpected( 
            UnexpectedEndOfInputDiagnostic( 
                front.getLocation() 
            ) 
        );
    }

    auto maybeLBrace = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::LBrace );
    if ( !maybeLBrace ) return std::unexpected( maybeLBrace.error() );

    if ( m_tokenStream.peek().checkMatches( TokenKind::Symbol, TokenSymbol::RBrace ) )
    {
        return std::unexpected(
            Diagnostic(
                "Import list cannot be empty",
                ErrorCategory::Syntax,
                ErrorSeverity::Error,
                front.getLocation()
            )
        );
    }

    std::vector<ImportedSymbol*> importedSymbols;

    while(true)
    {
        const Token& next = m_tokenStream.peek();

        if ( next.checkTypeMatches( TokenKind::EndOfFile )) {
            return std::unexpected( 
                UnexpectedEndOfInputDiagnostic( 
                    next.getLocation() 
                ) 
            );
        }

        if ( next.checkMatches( TokenKind::Symbol, TokenSymbol::RBrace ) )
        {
            // End of Symbol List
            auto maybeRBrace = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::RBrace );
            if ( !maybeRBrace ) return std::unexpected( maybeRBrace.error() );
            break;
        }

        auto maybeSymbol = parseSymbolImport();
        if ( !maybeSymbol ) return std::unexpected( maybeSymbol.error() );

        importedSymbols.push_back( std::move( maybeSymbol.value() ) );

        const Token& seperator = m_tokenStream.peek();

        if ( seperator.checkMatches( TokenKind::Symbol, TokenSymbol::Comma ) ) 
        {
            auto maybeCommaToken = m_tokenStream.expect( TokenKind::Symbol, TokenSymbol::Comma );
            if ( !maybeCommaToken ) return std::unexpected( maybeCommaToken.error() );
            continue;
        } 
        else if ( seperator.checkMatches( TokenKind::Symbol, TokenSymbol::RBrace ) ) 
        {
            // will be handled next loop iteration
            continue;
        } 
        else 
        {
            return std::unexpected(
                Diagnostic(
                    std::format(
                        "Expected ',' or '}}' after imported symbol, got '{}'", 
                        seperator.getValue() 
                    ), 
                    ErrorCategory::Syntax,
                    ErrorSeverity::Error,
                    seperator.getLocation()
                )
            );
        }
    }

    return importedSymbols;
}

std::expected<ImportedSymbol*, Diagnostic> StatementParser::parseSymbolImport()
{
    const Token& front = m_tokenStream.peek();

    if ( !front.checkTypeMatches( TokenKind::Identifier )) {
        return std::unexpected( 
            UnexpectedTypeDiagnostic(
                TokenKind::Identifier, 
                front.getType(), 
                front.getLocation() 
            ) 
        );
    }

    const Token& idToken = m_tokenStream.consume();

    Identifier* identifier = m_compUnit.allocate<Identifier>( idToken.getValue() );

    identifier->location = SourceRange::getLocation( idToken );

    std::optional<Identifier*> alias;

    if ( m_tokenStream.peek().checkMatches( TokenKind::Keyword, TokenKeyword::As ))
    {
        auto maybeAsKeyword = m_tokenStream.expect( TokenKind::Keyword, TokenKeyword::As );
        if ( !maybeAsKeyword ) return std::unexpected( maybeAsKeyword.error() );

        const Token& peekedToken = m_tokenStream.peek();

        if ( !peekedToken.checkTypeMatches( TokenKind::Identifier ) ) {
            return std::unexpected(
                UnexpectedTypeDiagnostic( 
                    TokenKind::Identifier, 
                    peekedToken.getType(), 
                    peekedToken.getLocation()
                )
            );
        }

        const Token& aliasToken = m_tokenStream.consume();

        Identifier* aliasIdentifier = m_compUnit.allocate<Identifier>( aliasToken.getValue() );

        aliasIdentifier->location = SourceRange::getLocation( aliasToken );

        alias = aliasIdentifier;
    }

    return m_compUnit.allocate<ImportedSymbol>( identifier, alias );
}