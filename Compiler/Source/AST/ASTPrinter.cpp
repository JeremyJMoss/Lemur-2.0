#include "AST/ASTPrinter.hpp"
#include <regex>
#include <fstream>

void ASTPrinter::writeIndent() const {
    static const char spaces[] =
        "                                                                "; // 64 spaces

    std::size_t count = m_indent * 2;
    while (count > sizeof(spaces) - 1)
    {
        m_out->write(spaces, sizeof(spaces) - 1);
        count -= sizeof(spaces) - 1;
    }
    m_out->write(spaces, count);
}

void ASTPrinter::startBlock() const {
    writeIndent(); 
    *m_out << "{\n";
}

void ASTPrinter::endBlock() const {
    writeIndent();
    *m_out << "}";
}

void ASTPrinter::increaseIndent() {
    m_indent++;
}

void ASTPrinter::decreaseIndent() {
    m_indent--;
}

void ASTPrinter::writeField(const std::string& label, const std::string& value, bool hasComma ) {
    writeIndent();

    *m_out << '"' << label << "\": \"" << value << '"';

    if ( hasComma )
        *m_out << ',';

    *m_out << '\n';
}

void ASTPrinter::writeNodeField( const std::string& label, const ASTNode& node, bool hasComma ) {
    writeIndent();
    *m_out << "\"" << label << "\": \n";
    increaseIndent();
    node.accept(*this);
    if ( hasComma ) {
        *m_out << ',';
    }
    *m_out << '\n';
    decreaseIndent();
}

void ASTPrinter::writeRawField( const std::string& label, const std::string& value, bool hasComma) {
    writeIndent();

    *m_out << '"' << label << "\": " << value;

    if ( hasComma )
        *m_out << ',';

    *m_out << '\n';
}

std::string ASTPrinter::getBinaryOperator( BinaryOperators op ) {
    switch ( op ) 
    {
        case BinaryOperators::Plus:                 return "+";
        case BinaryOperators::Minus:                return "-";
        case BinaryOperators::Multiply:             return "*";
        case BinaryOperators::Divide:               return "/";
        case BinaryOperators::LessThan:             return "<";
        case BinaryOperators::LessThanOrEqualTo:    return "<=";
        case BinaryOperators::GreaterThan:          return ">";
        case BinaryOperators::GreaterThanOrEqualTo: return ">=";
        case BinaryOperators::Equal:                return "==";
        case BinaryOperators::NotEqual:             return "!=";
        case BinaryOperators::And:                  return "&&";
        case BinaryOperators::Or:                   return "||";
        case BinaryOperators::Remainder:            return "%";
        default:                                    return "Unknown";
    }
}

std::string ASTPrinter::getCharToString( char c )
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
            return "'" + std::string(1, c) + "'";
    }
}

std::string ASTPrinter::getParsedType( const ParsedTypeKind& type ) {
    switch ( type ) 
    {
        case ParsedTypeKind::Function:   return "Function";
        case ParsedTypeKind::Ownership:  return "Ownership";
        case ParsedTypeKind::Inferred:   return "Inferred";
        case ParsedTypeKind::Array:      return "Array";
        case ParsedTypeKind::Named:      return "Named";
        default:               return "Unknown";
    }
}

std::string ASTPrinter::getOwnershipKind( const OwnershipKind kind ) {
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

std::string ASTPrinter::trimTrailingZeros(const std::string& str)
{
    static const std::regex pattern(
        R"((\.\d*?[1-9])0+|\.(?=0+$)|\.0+$)"
    );

    return std::regex_replace(str, pattern, "$1");
}

std::string ASTPrinter::getLiteralValue( const LiteralValue& value )
{
    return std::visit( [] ( const auto& val ) -> std::string
    {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr ( std::is_same_v<T, bool> ) return val ? "true" : "false";

        else if constexpr ( std::is_same_v<T, int> ) return std::to_string( val );

        else if constexpr ( std::is_same_v<T, float> ) return trimTrailingZeros( std::to_string( val ) );

        else if constexpr ( std::is_same_v<T, std::string> ) return "\"" + val + "\"";

        else if constexpr ( std::is_same_v<T, char> ) return getCharToString( val );

        else if constexpr ( std::is_same_v<T, std::monostate> ) return "null";
        
        throw std::runtime_error( "Weird string literal" );

    }, value );
}

std::string ASTPrinter::getUnaryOperator( const UnaryOperator& unop ) {
    switch( unop ) 
    {
        case UnaryOperator::Negate: return "-";
        case UnaryOperator::Not:    return "!";
        default:                    return "Unknown";
    }
}

void ASTPrinter::visit( const ParsedType& parsedType ) {
    switch ( parsedType.kind ) 
    {
        case ParsedTypeKind::Function: 
        {
            auto functionType = static_cast<const ParsedFunctionType*>( &parsedType );
            startBlock();
            increaseIndent();
            writeField( "id", functionType->id );
            writeField( "type", getParsedType( functionType->kind ) );
            writeNodeField( "returnType", *functionType->returnType );
            writeArrayField( "parameters", functionType->parameters );
            decreaseIndent();
            endBlock();
            break;
        }

        case ParsedTypeKind::Inferred: 
        {
            auto inferredType = static_cast<const ParsedInferredType*>( &parsedType );
            startBlock();
            increaseIndent();
            writeField( "id", inferredType->id );
            writeField( "type", getParsedType( inferredType->kind ) );
            decreaseIndent();
            endBlock();
            break;
        }

        case ParsedTypeKind::Ownership: 
        {
            auto ownershipType = static_cast<const ParsedOwnershipType*>( &parsedType );
            startBlock();
            increaseIndent();
            writeField( "id", ownershipType->id );
            writeField( "type", getParsedType( ownershipType->kind ) );
            writeField( "ownership", getOwnershipKind( ownershipType->ownership ) );
            writeNodeField( "inner", *ownershipType->inner, false );
            decreaseIndent();
            endBlock();
            break;
        }

        case ParsedTypeKind::Array: 
        {
            auto arrayType = static_cast<const ParsedArrayType*>( &parsedType );
            startBlock();
            increaseIndent();
            writeField( "id", arrayType->id );
            writeField( "type", getParsedType( arrayType->kind ) );
            writeNodeField( "elementType", *arrayType->elementType );
            writeNodeField( "arraySize", *arrayType->size, false );
            decreaseIndent();
            endBlock();
            break;
        }

        case ParsedTypeKind::Named:
        {
            auto namedType = static_cast<const ParsedNamedType*>( &parsedType );
            startBlock();
            increaseIndent();
            writeField( "id", namedType->id );
            writeField( "type", getParsedType( namedType->kind ) );
            writeNodeField( "identifier", *namedType->identifier, false );
            decreaseIndent();
            endBlock();
            break;
        }

        default: 
        {
            writeIndent();
            *m_out << "\"Unknown Type Reference\"\n";
            break;
        }
    }
}

void ASTPrinter::visit( const Literal& literal ) {
    startBlock();
    increaseIndent();
    writeField("id", literal.id);
    writeField("type", std::string("Literal" ) );
    writeRawField("value", getLiteralValue( literal.value ), false);
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit( const Assignment& assignment ) {
    startBlock();
    increaseIndent();
    writeField( "id", assignment.id );
    writeField( "type", std::string("Assignment" ) );
    writeNodeField( "identifier", *assignment.identifier );
    writeNodeField( "value", *assignment.value, false );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit( const Identifier& identifier ) {
    startBlock();
    increaseIndent();
    writeField( "id", identifier.id );
    writeField( "type", std::string("Identifier" ) );
    writeField( "name", identifier.name, false );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit( const ExpressionStatement& expressionStmt ) {
    startBlock();
    increaseIndent();
    writeField( "id", expressionStmt.id );
    writeField( "type", std::string("Expression Statement" ) );
    writeNodeField( "expression", *expressionStmt.expression, false );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit( const BinaryExpression& binExp ) {
    startBlock();
    increaseIndent();
    writeField( "id", binExp.id );
    writeField( "type", std::string("Binary Expression") );
    writeField( "op", getBinaryOperator( binExp.op ) );
    writeNodeField( "left", *binExp.left );
    writeNodeField( "right", *binExp.right, false );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit(const Block& block ) {
    startBlock();
    increaseIndent();
    writeField( "id", block.id );
    writeField( "type", std::string("Block" ) );
    writeArrayField( "statements", block.statements, false );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit( const Break& breakStmt ) {
    startBlock();
    increaseIndent();
    writeField( "id", breakStmt.id );
    writeField( "type", std::string("Break" ) );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit( const Continue& continueStmt ) {
    startBlock();
    increaseIndent();
    writeField( "id", continueStmt.id );
    writeField( "type", std::string("Continue" ) );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit( const Return& returnStmt ) {
    startBlock();
    increaseIndent();
    writeField( "id", returnStmt.id );
    writeField( "type", std::string("Return" ) );
    writeNodeField( "value", *returnStmt.value, false );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit( const VariableDeclaration& varDec ) {
    startBlock();
    increaseIndent();
    writeField( "id", varDec.id );
    writeField( "type", std::string("Variable Declaration" ) );
    writeRawField( "locked", varDec.locked ? "true" : "false" );
    writeNodeField( "varType", *varDec.varType );
    writeNodeField( "identifier", *varDec.identifier );
    writeNodeField( "initialiser", *varDec.initialiser, false );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit( const FunctionDeclaration& funDec ) {
    startBlock();
    increaseIndent();
    writeField( "id", funDec.id );
    writeField( "type", std::string("Function Declaration" ) );
    writeNodeField( "returnType", *funDec.returnType );
    writeArrayField( "parameters", funDec.parameters );
    writeNodeField( "body", *funDec.body, false );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit(const FunctionLiteral& funLit ) {
    startBlock();
    increaseIndent();
    writeField( "id", funLit.id );
    writeField( "type", std::string("Function Literal" ) );
    writeNodeField( "returnType", *funLit.returnType );
    writeArrayField( "parameters", funLit.parameters );
    writeNodeField( "body", *funLit.body, false );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit(const IfConditional& ifCond ) {
    startBlock();
    increaseIndent();
    writeField( "id", ifCond.id );
    writeField( "type", std::string("If Statement" ) );
    writeNodeField( "condition", *ifCond.condition);
    writeNodeField( "then", *ifCond.then );
    writeNodeField( "else", *ifCond.elseStatement, false );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit(const ForLoop& forl ) {
    startBlock();
    increaseIndent();
    writeField( "id", forl.id );
    writeField( "type", std::string("For Loop" ) );
    writeNodeField( "loopVar", *forl.loopVar);
    writeNodeField( "iterable", *forl.iterable );
    writeNodeField( "step", *forl.step );
    writeNodeField( "condition", *forl.condition );
    writeNodeField( "body", *forl.body, false );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit( const Range& range ) {
    startBlock();
    increaseIndent();
    writeField( "id", range.id );
    writeField( "type", std::string("Range" ) );
    writeNodeField( "start", *range.start);
    writeNodeField( "end", *range.end );
    writeRawField( "locked", range.inclusive ? "true" : "false", false );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit( const Unary& unary ) {
    startBlock();
    increaseIndent();
    writeField( "id", unary.id );
    writeField( "type", std::string("Unary" ) );
    writeField( "op", getUnaryOperator( unary.op ) );
    writeNodeField( "argument", *unary.argument, false );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit( const FunctionCall& funCall ) {
    startBlock();
    increaseIndent();
    writeField( "id", funCall.id );
    writeField( "type", std::string("Function Call" ) );
    writeNodeField( "callee", *funCall.callee );
    writeArrayField( "arguments", funCall.arguments, false );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit( const Parameter& parameter ) {
    startBlock();
    increaseIndent();
    writeField( "id", parameter.id );
    writeField( "type", std::string("Parameter" ) );
    writeNodeField( "identifier", *parameter.identifier );
    if ( parameter.defaultValue != nullptr ) {
        writeNodeField( "defaultValue", *parameter.defaultValue );
    }
    writeNodeField( "paramType", *parameter.paramType, false );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::print( const std::vector<const Statement*>& statements )
{
    std::ofstream outFile("ast_output.json", std::ios::out | std::ios::binary);
    outFile.rdbuf()->pubsetbuf(nullptr, 1 << 20); // 1MB buffer

    m_out = &outFile;

    *m_out << "[\n";

    for ( std::size_t i = 0; i < statements.size(); ++i ) 
    {
        if (i != 0) {
            *m_out << ",\n";
        }
        increaseIndent();
        statements[i]->accept(*this);
        decreaseIndent();
    }

    *m_out << "\n]\n";

    outFile.close();
}