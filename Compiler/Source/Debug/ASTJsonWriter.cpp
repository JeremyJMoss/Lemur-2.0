/* === Main Import === */

#include "Debug/ASTJsonWriter.hpp"

/* === Dependencies === */

#include <fstream>
#include <filesystem>
#include <format>

namespace fs = std::filesystem;

/* === Imports === */

#include "AST/AllASTTypes.hpp"
#include "Errors/Errors.hpp"

/* === AST Json Writer Methods === */

void ASTJsonWriter::writeNodeField( std::string_view label, const ASTNode& node, bool hasComma ) {
    m_writer.writeIndent();
    m_writer.writeRaw( std::format("\"{}\": \n", label) );
    m_writer.increaseIndent();
    node.accept( *this );
    if ( hasComma ) {
        m_writer.writeRaw(",");
    }
    m_writer.writeNewLine();
    m_writer.decreaseIndent();
}

std::string ASTJsonWriter::getBinaryOperator( BinaryOperator op ) {
    switch ( op ) 
    {
        case BinaryOperator::Plus:                 return "+";
        case BinaryOperator::Minus:                return "-";
        case BinaryOperator::Multiply:             return "*";
        case BinaryOperator::Divide:               return "/";
        case BinaryOperator::LessThan:             return "<";
        case BinaryOperator::LessThanOrEqualTo:    return "<=";
        case BinaryOperator::GreaterThan:          return ">";
        case BinaryOperator::GreaterThanOrEqualTo: return ">=";
        case BinaryOperator::Equal:                return "==";
        case BinaryOperator::NotEqual:             return "!=";
        case BinaryOperator::And:                  return "&&";
        case BinaryOperator::Or:                   return "||";
        case BinaryOperator::Remainder:            return "%";
        default:                                   return "Unknown";
    }
}

std::string ASTJsonWriter::getCharToString( char c )
{
    switch (c) {
        case '\n': return "'\\n'";
        case '\t': return "'\\t'";
        case '\r': return "'\\r'";
        case '\0': return "'\\0'";
        case '\\': return "'\\\\'";
        case '\'': return "'\\\''";
        case '\"': return "'\\\"'";
        default:
            return "'" + std::string( 1, c ) + "'";
    }
}

std::string ASTJsonWriter::getParsedType( const ParsedTypeKind& type ) {
    switch ( type ) 
    {
        case ParsedTypeKind::Function:   return "Function";
        case ParsedTypeKind::Ownership:  return "Ownership";
        case ParsedTypeKind::Inferred:   return "Inferred";
        case ParsedTypeKind::Array:      return "Array";
        case ParsedTypeKind::Named:      return "Named";
        default:                         return "Unknown";
    }
}

std::string ASTJsonWriter::getOwnershipKind( const OwnershipKind kind ) {
    switch ( kind ) 
    {
        case OwnershipKind::Owned:       return "Exclusive Pointer";
        case OwnershipKind::Rref:        return "Immutable Reference";
        case OwnershipKind::Wref:        return "Mutable Reference";
        case OwnershipKind::Shared:      return "Shareable Pointer";
        case OwnershipKind::Weak:        return "Weak Pointer";
        case OwnershipKind::None:        return "No Ownership";
        default:                         return "Unknown";
    }
};

std::string ASTJsonWriter::trimTrailingZeros( std::string_view str )
{
    std::string result( str );

    auto dot = result.find('.');
    if (dot == std::string::npos)
        return result;

    while (!result.empty() && result.back() == '0')
        result.pop_back();

    if (!result.empty() && result.back() == '.')
        result.pop_back();

    return result;
}

std::string ASTJsonWriter::getLiteralValue( const LiteralValue& value )
{
    return std::visit( [] ( const auto& val ) -> std::string
    {
        using T = std::decay_t<decltype( val )>;
        
        if constexpr ( std::is_same_v<T, bool> ) return val ? "true" : "false";

        else if constexpr ( std::is_same_v<T, int> ) return std::to_string( val );

        else if constexpr ( std::is_same_v<T, float> ) return trimTrailingZeros( std::to_string( val ) );

        else if constexpr ( std::is_same_v<T, std::string> ) return std::format("\"{}\"", val );

        else if constexpr ( std::is_same_v<T, char> ) return getCharToString( val );

        else if constexpr ( std::is_same_v<T, std::monostate> ) return "null";
        
        throw InternalCompilerError( "Unclassified literal value.\nPlease report this bug." );

    }, value );
}

std::string ASTJsonWriter::getUnaryOperator( UnaryOperator unop ) {
    switch( unop ) 
    {
        case UnaryOperator::Negate: return "-";
        case UnaryOperator::Not:    return "!";
        default:                    return "Unknown";
    }
}

void ASTJsonWriter::visit( const ParsedType& parsedType ) {
    switch ( parsedType.kind ) 
    {
        case ParsedTypeKind::Function: 
        {
            auto functionType = static_cast<const ParsedFunctionType*>( &parsedType );
            m_writer.startBlock();
            m_writer.increaseIndent();
            m_writer.writeField( "id", functionType->id.value );
            m_writer.writeField( "type", getParsedType( functionType->kind ) );
            writeNodeField( "returnType", *functionType->returnType );
            m_writer.writeArrayField( 
                "parameters", 
                functionType->parameters, 
                [&]( const ParsedType* type )
                {
                    type->accept( *this );
                },
                false 
            );
            m_writer.decreaseIndent();
            m_writer.endBlock();
            break;
        }

        case ParsedTypeKind::Inferred: 
        {
            auto inferredType = static_cast<const ParsedInferredType*>( &parsedType );
            m_writer.startBlock();
            m_writer.increaseIndent();
            m_writer.writeField( "id", inferredType->id.value );
            m_writer.writeField( "type", getParsedType( inferredType->kind ), false );
            m_writer.decreaseIndent();
            m_writer.endBlock();
            break;
        }

        case ParsedTypeKind::Ownership: 
        {
            auto ownershipType = static_cast<const ParsedOwnershipType*>( &parsedType );
            m_writer.startBlock();
            m_writer.increaseIndent();
            m_writer.writeField( "id", ownershipType->id.value );
            m_writer.writeField( "type", getParsedType( ownershipType->kind ) );
            m_writer.writeField( "ownership", getOwnershipKind( ownershipType->ownership ) );
            writeNodeField( "inner", *ownershipType->inner, false );
            m_writer.decreaseIndent();
            m_writer.endBlock();
            break;
        }

        case ParsedTypeKind::Array: 
        {
            auto arrayType = static_cast<const ParsedArrayType*>( &parsedType );
            m_writer.startBlock();
            m_writer.increaseIndent();
            m_writer.writeField( "id", arrayType->id.value );
            m_writer.writeField( "type", getParsedType( arrayType->kind ) );
            writeNodeField( "elementType", *arrayType->elementType );
            writeNodeField( "arraySize", *arrayType->size, false );
            m_writer.decreaseIndent();
            m_writer.endBlock();
            break;
        }

        case ParsedTypeKind::Named:
        {
            auto namedType = static_cast<const ParsedNamedType*>( &parsedType );
            m_writer.startBlock();
            m_writer.increaseIndent();
            m_writer.writeField( "id", namedType->id.value );
            m_writer.writeField( "type", getParsedType( namedType->kind ) );
            writeNodeField( "identifier", *namedType->identifier, false );
            m_writer.decreaseIndent();
            m_writer.endBlock();
            break;
        }

        default: 
        {
            m_writer.writeIndent();
            m_writer.writeRaw("\"Unknown Type Reference\"\n");
            break;
        }
    }
}

void ASTJsonWriter::visit( const Literal& literal ) {
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", literal.id.value );
    m_writer.writeField( "type", std::string_view( "Literal" ) );
    m_writer.writeRawField( "value", getLiteralValue( literal.value ), false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit( const Assignment& assignment ) {
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", assignment.id.value );
    m_writer.writeField( "type", std::string_view( "Assignment" ) );
    writeNodeField( "identifier", *assignment.identifier );
    writeNodeField( "value", *assignment.value, false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit( const Identifier& identifier ) {
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", identifier.id.value );
    m_writer.writeField( "type", std::string_view( "Identifier" ) );
    m_writer.writeField( "name", identifier.name, false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit( const ExpressionStatement& expressionStmt ) {
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", expressionStmt.id.value );
    m_writer.writeField( "type", std::string_view( "Expression Statement" ) );
    writeNodeField( "expression", *expressionStmt.expression, false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit( const BinaryExpression& binExp ) {
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", binExp.id.value );
    m_writer.writeField( "type", std::string_view( "Binary Expression" ) );
    m_writer.writeField( "op", getBinaryOperator( binExp.op ) );
    writeNodeField( "left", *binExp.left );
    writeNodeField( "right", *binExp.right, false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit(const Block& block ) {
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", block.id.value );
    m_writer.writeField( "type", std::string_view( "Block" ) );
    m_writer.writeArrayField( 
        "statements", 
        block.statements,
        [&]( const Statement* stmt )
        {
            stmt->accept( *this );
        }, 
        false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit(const BlockStatement& blockStmt ) {
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", blockStmt.id.value );
    m_writer.writeField( "type", std::string_view( "Block Statement" ) );
    writeNodeField( "block", *blockStmt.block, false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit( const Break& breakStmt ) {
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", breakStmt.id.value );
    m_writer.writeField( "type", std::string_view( "Break" ) );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit( const Continue& continueStmt ) {
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", continueStmt.id.value );
    m_writer.writeField( "type", std::string_view( "Continue" ) );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit( const Return& returnStmt ) {
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", returnStmt.id.value );
    m_writer.writeField( "type", std::string_view( "Return" ) );
    writeNodeField( "value", *returnStmt.value, false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit( const VariableDeclaration& varDec ) {
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", varDec.id.value );
    m_writer.writeField( "type", std::string_view( "Variable Declaration" ) );
    m_writer.writeField( "exported", varDec.visibility == DeclarationVisibility::Private ? "false" : "true" );
    m_writer.writeRawField( "locked", varDec.locked ? "true" : "false" );
    writeNodeField( "varType", *varDec.varType );
    writeNodeField( "identifier", *varDec.identifier );
    writeNodeField( "initialiser", *varDec.initialiser, false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit( const FunctionDeclaration& funDec ) {
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", funDec.id.value );
    m_writer.writeField( "type", std::string_view( "Function Declaration" ) );
    m_writer.writeField( "exported", funDec.visibility == DeclarationVisibility::Private ? "false" : "true" );
    writeNodeField( "returnType", *funDec.returnType );
    m_writer.writeArrayField( 
        "parameters", 
        funDec.parameters,
        [&]( const Parameter* parameter )
        {
            parameter->accept( *this );
        }
    );
    writeNodeField( "body", *funDec.body, false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit(const FunctionLiteral& funLit ) {
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", funLit.id.value );
    m_writer.writeField( "type", std::string_view( "Function Literal" ) );
    writeNodeField( "returnType", *funLit.returnType );
    m_writer.writeArrayField( 
        "parameters", 
        funLit.parameters,
        [&]( const Parameter* param )
        {
            param->accept( *this );
        }
    );
    writeNodeField( "body", *funLit.body, false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit(const IfConditional& ifCond ) {
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", ifCond.id.value );
    m_writer.writeField( "type", std::string_view( "If Statement" ) );
    writeNodeField( "condition", *ifCond.condition);
    writeNodeField( "then", *ifCond.then );
    writeNodeField( "else", *ifCond.elseStatement, false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit(const ForLoop& forl ) {
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", forl.id.value );
    m_writer.writeField( "type", std::string_view( "For Loop" ) );
    writeNodeField( "loopVar", *forl.loopVar);
    writeNodeField( "iterable", *forl.iterable );
    writeNodeField( "step", *forl.step );
    writeNodeField( "condition", *forl.condition );
    writeNodeField( "body", *forl.body, false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit( const Range& range ) 
{
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", range.id.value );
    m_writer.writeField( "type", std::string_view( "Range" ) );
    writeNodeField( "start", *range.start);
    writeNodeField( "end", *range.end );
    m_writer.writeRawField( "locked", range.inclusive ? "true" : "false", false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit( const Unary& unary ) 
{
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", unary.id.value );
    m_writer.writeField( "type", std::string_view( "Unary" ) );
    m_writer.writeField( "op", getUnaryOperator( unary.op ) );
    writeNodeField( "argument", *unary.argument, false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit( const FunctionCall& funCall ) 
{
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", funCall.id.value );
    m_writer.writeField( "type", std::string_view( "Function Call" ) );
    writeNodeField( "callee", *funCall.callee );
    m_writer.writeArrayField( 
        "arguments", 
        funCall.arguments, 
        [&]( const Expression* expr )
        {
            expr->accept( *this );
        },
        false 
    );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit( const Parameter& parameter ) 
{
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", parameter.id.value );
    m_writer.writeField( "type", std::string_view( "Parameter" ) );
    writeNodeField( "identifier", *parameter.identifier );
    if ( parameter.defaultValue != nullptr ) {
        writeNodeField( "defaultValue", *parameter.defaultValue );
    }
    writeNodeField( "paramType", *parameter.paramType, false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit( const ModuleDeclaration& modDec ) 
{
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", modDec.id.value );
    m_writer.writeField( "type", std::string_view( "Module Declaration" ) );
    writeNodeField( "name", *modDec.name, false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit( const Import& import ) 
{
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", import.id.value );
    m_writer.writeField( "type", std::string_view( "Import" ) );
    writeNodeField( "moduleName", *import.moduleName, import.alias.has_value() || !import.importedSymbols.empty() );
    if ( import.alias.has_value() ) 
    {
        writeNodeField( "alias", *import.alias.value(), import.importedSymbols.empty() );
    }
    if ( !import.importedSymbols.empty() )
    {
        m_writer.writeArrayField( 
            "importedSymbols", 
            import.importedSymbols,
            [&]( const ImportedSymbol* sym )
            {
                sym->accept( *this );
            }, 
            false 
        );
    }
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit( const ImportedSymbol& importSymbol )
{
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", importSymbol.id.value );
    m_writer.writeField( "type", std::string_view( "Import Symbol" ) );
    writeNodeField( "name", *importSymbol.name, importSymbol.alias.has_value() );
    if ( importSymbol.alias.has_value() )
    {
        writeNodeField( "alias", *importSymbol.alias.value(), false );
    }
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::visit( const QualifiedName& qualName )
{
    m_writer.startBlock();
    m_writer.increaseIndent();
    m_writer.writeField( "id", qualName.id.value );
    m_writer.writeField( "type", std::string_view( "Qualified Name" ) );
    m_writer.writeField( "name", qualName.name, false );
    m_writer.decreaseIndent();
    m_writer.endBlock();
}

void ASTJsonWriter::write( const std::vector<const Statement*>& statements, const fs::path& outputPath, const std::string_view moduleName )
{
    // Create the directory (and any missing parent directories)
    std::error_code ec;
    fs::create_directories( outputPath, ec );

    if ( ec ) throw InternalCompilerError( "Failed to create output directory: " + ec.message() );

    std::string fileName = std::string( moduleName ) + "_ast_output.json";
    std::ofstream outFile( outputPath / fileName, std::ios::out | std::ios::binary );

    if ( !outFile ) throw InternalCompilerError( "Failed to open output file." );

    // Create 1MB buffer
    outFile.rdbuf()->pubsetbuf( nullptr, 1 << 20 );

    m_writer.setOutFile( outFile );

    m_writer.writeRaw("[");
    m_writer.writeNewLine();

    for ( std::size_t i = 0; i < statements.size(); ++i ) 
    {
        if ( i != 0 ) {
            m_writer.writeRaw(",");
            m_writer.writeNewLine();
        }
        m_writer.increaseIndent();
        statements[i]->accept( *this );
        m_writer.decreaseIndent();
    }
    
    m_writer.writeNewLine();
    m_writer.writeRaw("]");
    m_writer.writeNewLine();

    outFile.close();
}