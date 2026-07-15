#pragma once

/* === Dependencies ===*/

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
        fs::path m_filePath;
        std::vector<std::streampos> m_lineOffsets; // byte offset at start of each line

    public:
        FileId getFileId() const 
        { 
            return m_id; 
        }

        fs::path getFilePath() const 
        { 
            return m_filePath; 
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