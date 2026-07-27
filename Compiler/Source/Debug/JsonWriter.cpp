/* === Main Import === */

#include "Debug/JsonWriter.hpp"

void JsonWriter::writeIndent() const {
    static const char spaces[] = "                                                                "; // 64 spaces

    std::size_t count = m_indent * 2;
    while ( count > sizeof( spaces ) - 1 )
    {
        m_out->write( spaces, sizeof( spaces ) - 1 );
        count -= sizeof( spaces ) - 1;
    }
    m_out->write( spaces, count );
}

void JsonWriter::startBlock() const {
    writeIndent(); 
    *m_out << "{\n";
}

void JsonWriter::endBlock() const {
    writeIndent();
    *m_out << "}";
}

void JsonWriter::increaseIndent() {
    m_indent++;
}

void JsonWriter::decreaseIndent() {
    m_indent--;
}

void JsonWriter::writeRawField( std::string_view label, std::string_view value, bool hasComma ) {
    writeIndent();

    *m_out << '"' << label << "\": " << value;

    if ( hasComma )
        *m_out << ',';

    *m_out << '\n';
}

void JsonWriter::writeRaw( std::string_view value )
{
    *m_out << value;
}

void JsonWriter::writeNewLine()
{
    *m_out << '\n';
}

void JsonWriter::setOutFile( std::ostream& outFile )
{
    m_out = &outFile;
}