#ifndef HFDP_BUFFER
#define HFDP_BUFFER

#include <cstddef>
#include <queue>

#define HFDP_DEFAULT_BUFFER_SIZE 2048 * 32

namespace HFDP {
    namespace Buffer {

        struct CircBufData {
            char* begin;
            std::size_t size;
        };

        class Buffer {
        public:
            Buffer(std::size_t buffSize);
            ~Buffer() { delete m_CircularBufBegin; };
        public:
            std::size_t popPacket(char* dst);
            void pushPacket(char* src, std::size_t dataSize);
        private:
            inline char* pushBuffer(char* src_buf, std::size_t size);
            inline void popBuffer(char* dst_buf, char* begin_ptr, std::size_t size);

        private:
            std::size_t m_BuffSize = 0;
            char *m_CircularBufBegin, *m_CircularBufEnd, *m_CircularBufCurr;
            std::queue<CircBufData> dataQueue;
        };
    }
}

#endif