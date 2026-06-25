#include <vector>

class Arena {
    public:
        template <typename T, typename... Args>
        T* create( Args&&... args ) {
            std::size_t aligned = alignUp( m_offset, alignof(T) );

            if ( aligned + sizeof(T) > m_chunks.back().size() ) {
                addChunk();
                aligned = m_offset = 0;
            }

            void* ptr = m_chunks.back().data() + aligned;
            m_offset = aligned + sizeof( T );

            return new ( ptr ) T( std::forward<Args>( args )... );
        }

        void reset() {
            m_chunks.clear();
            addChunk();
        }

        explicit Arena( std::size_t chunkSize = 1024 * 1024 ) : m_chunkSize( chunkSize ) { 
            addChunk(); 
        }

    private:
        std::vector<std::vector<std::byte>> m_chunks;
        std::size_t m_chunkSize;
        std::size_t m_offset = 0;

        void addChunk() {
            m_chunks.emplace_back( m_chunkSize );
            m_offset = 0;
        }

        static std::size_t alignUp(std::size_t n, std::size_t align) {
            return (n + align - 1) & ~(align - 1);
        }
};