#include "buffer.h"


namespace HFDP {
    namespace Buffer {
        Buffer::Buffer(std::size_t buffSize): m_BuffSize(buffSize)
        {
            m_CircularBufBegin = new char[m_BuffSize];
            m_CircularBufCurr = m_CircularBufBegin;
            m_CircularBufEnd = m_CircularBufBegin + m_BuffSize;
        }

        void Buffer::pushPacket(char* src, std::size_t dataSize)
        {
            CircBufData packet;
            packet.begin = pushBuffer(src, dataSize);
            packet.size = dataSize;
            dataQueue.push(packet);
        }

        std::size_t Buffer::popPacket(char* dst)
        {
            if(dataQueue.empty()) {
                return 0;
            }
            CircBufData packet = dataQueue.front();
            popBuffer(dst, packet.begin, packet.size);
            dataQueue.pop();
            return packet.size;
        }

        inline char* Buffer::pushBuffer(char* src, std::size_t size)
        {
            if(m_CircularBufCurr + size > m_CircularBufEnd)
                m_CircularBufCurr = m_CircularBufBegin;
            
            std::memcpy(m_CircularBufCurr, src, size);

            char* temp = m_CircularBufCurr;
            m_CircularBufCurr += size;
            return temp;
        }

        inline void Buffer::popBuffer(char* dst_buf, char* begin_ptr, std::size_t size)
        {
            std::memcpy(dst_buf, begin_ptr, size);
        }
    }
}