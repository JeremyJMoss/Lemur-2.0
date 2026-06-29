#include "AST/ASTPrinter.hpp"
#include <regex>

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
    if ( hasTrailingComma ) *m_out << ",";
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
    writeIndent();
    *m_out << "\"id\": \"" << literal.id << "\"," << '\n';
    m_indent++;
    writeIndent();
    *m_out << "\"type\": \"Literal\"," << '\n';
    writeIndent();
    m_indent--;
    *m_out << "\"value\": " << getLiteralValue( literal.value ) << '\n';
    endBlock();
}

void ASTPrinter::visit( const Assignment& assignment ) {
    startBlock();
    m_indent++;
    writeIndent();
    *m_out << "\"id\": \"" << assignment.id << "\",\n";
    writeIndent();
    *m_out << "\"type\": \"Assignment\",\n";
    writeIndent();
    *m_out << "\"identifier\":\n";
    m_indent++;
    assignment.identifier->accept(*this);
    m_indent--;
    writeIndent();
    *m_out << "\"value\":\n";
    m_indent++;
    assignment.value->accept(*this);
    m_indent--;
    endBlock();
}