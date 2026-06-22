#include <vector>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

using FileId = size_t;

class FileData
{
    private:
        static inline FileId s_nextId = 0;
        FileId m_id;
        fs::path m_filePath;
        std::vector<std::streampos> m_lineOffsets; // byte offset at start of each line

    public:
        FileId getFileId(){ return m_id; }

        fs::path getFilePath() { return m_filePath; }

        void addLineOffset( std::streampos pos ) { m_lineOffsets.push_back( pos ); }

        size_t getLinesCollected() { return m_lineOffsets.size(); }

        std::streampos getStreamPosition( std::size_t lineNo ) { return m_lineOffsets[ lineNo - 1 ]; }

        FileData( fs::path filePath ) : m_id( s_nextId++ ), m_filePath( filePath ) {}
};