#pragma once

/* === Dependencies === */

#include <fstream>
#include <vector>
#include <string>

/* === Json Writer === */

class JsonWriter
{
    private:
        std::ostream* m_out;
        std::int16_t m_indent = 0;
    public:
        void setOutFile( std::ostream& outFile );
        void writeIndent() const;
        void startBlock() const;
        void endBlock() const;

        void increaseIndent();
        void decreaseIndent();

        void writeRaw( std::string_view value );
        void writeNewLine();

        template <typename T>
        void writeField( std::string_view label, const T& value, bool hasComma = true )
        {
            writeIndent();

            *m_out << '"' << label << "\": ";

            if constexpr ( std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view> )
            {
                *m_out << '"' << value << '"';
            }
            else
            {
                *m_out << value;
            }

            if (hasComma)
                *m_out << ',';

            *m_out << '\n';
        }

        template <typename T, typename Writer>
        void writeArrayField(
            std::string_view label,
            const std::vector<T>& vec,
            Writer writer,
            bool hasComma = true
        )
        {
            writeIndent();
            *m_out << '"' << label << "\": [";

            if (!vec.empty())
            {
                *m_out << '\n';
                increaseIndent();

                for (std::size_t i = 0; i < vec.size(); ++i)
                {
                    if (i != 0)
                        *m_out << ",\n";

                    writer(vec[i]);
                }

                decreaseIndent();
                *m_out << '\n';
                writeIndent();
            }

            *m_out << "]";

            if (hasComma)
                *m_out << ',';

            *m_out << '\n';
        }

        void writeRawField( std::string_view label, std::string_view value, bool hasComma = true );
};