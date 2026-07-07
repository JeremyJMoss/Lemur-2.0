#pragma once

/* === Imports === */

#include <vector>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

using FileId = size_t;

/* === File Data === */

class FileData
{
    private:
        static inline FileId s_nextId = 0;
        FileId m_id;
        std::string_view m_moduleName;
        fs::path m_filePath;
        std::vector<std::streampos> m_lineOffsets; // byte offset at start of each line

    public:
        FileId getFileId() const 
        { 
            return m_id; 
        }

        std::string_view getModuleName() const
        {
            return m_moduleName;
        }

        fs::path getFilePath() const 
        { 
            return m_filePath; 
        }

        void setModuleName( std::string_view moduleName ) 
        {
            m_moduleName = moduleName;
        }

        void addLineOffset( std::streampos pos ) 
        { 
            m_lineOffsets.push_back( pos ); 
        }

        size_t getLinesCollected() const 
        { 
            return m_lineOffsets.size(); 
        }

        std::streampos getStreamPosition( std::size_t lineNo ) const 
        { 
            return m_lineOffsets[ lineNo - 1 ]; 
        }

        FileData( fs::path filePath ) 
            : m_id( s_nextId++ ), 
              m_filePath( filePath ) {}
};