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

void ASTPrinter::writeField(const std::string& label, const std::string& value, bool hasComma = true ) {
    writeIndent();

    *m_out << '"' << label << "\": \"" << value << '"';

    if ( hasComma )
        *m_out << ',';

    *m_out << '\n';
}

std::string ASTPrinter::getBinaryOperator( BinaryOperators op ) const {
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

std::string ASTPrinter::trimTrailingZeros(const std::string& str)
{
    static const std::regex pattern(
        R"((\.\d*?[1-9])0+|\.(?=0+$)|\.0+$)"
    );

    return std::regex_replace(str, pattern, "$1");
}

std::string ASTPrinter::getLiteralValue( const LiteralValue& value ) const
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

void ASTPrinter::visit( const Literal& literal ) {
    startBlock();
    increaseIndent();
    writeField("id", literal.value);
    writeField("type", "Literal");
    writeField("value", getLiteralValue( literal.value ), false);
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit( const Assignment& assignment ) {
    startBlock();
    increaseIndent();
    writeField( "id", assignment.id );\
    writeField( "type", "Assignment" );
    writeIndent();
    *m_out << "\"identifier\":\n";
    increaseIndent();
    assignment.identifier->accept( *this );
    decreaseIndent();
    *m_out << ",\n";
    writeIndent();
    *m_out << "\"value\":\n";
    increaseIndent();
    assignment.value->accept(*this);
    decreaseIndent();
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit( const Identifier& identifier ) {
    startBlock();
    increaseIndent();
    writeField( "id", identifier.id );
    writeField( "type", "Identifier" );
    writeField( "name", identifier.name, false );
    decreaseIndent();
    endBlock();
}

void ASTPrinter::visit( const ExpressionStatement& expressionStmt ) {
    startBlock();
    increaseIndent();
    writeField( "id", expressionStmt.id );
    writeField( "type", "Expression Statement" );
    writeIndent();
    *m_out << "\"expression\": \n";
    increaseIndent();
    expressionStmt.expression->accept(*this);
    decreaseIndent();
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
        statements[i]->accept(*this);
    }

    *m_out << "\n]\n";

    outFile.close();
}