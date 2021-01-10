#ifndef HFDP_FEC_MANAGER
#define HFDP_FEC_MANAGER

#include "dataPacket.h"
#include "../../lib/queue/blockingconcurrentqueue.h"
#include "hfdpSocket.h"


namespace HFDP {
    class FecManager {
    public:
        FecManager(std::shared_ptr<HFDP_Socket> dataSock);
        ~FecManager();
    public:
        void startFec();
        inline void setRxQueue(std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> queue) { m_queue_in = queue; };
        inline std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> getTxQueue() { return m_queue_out; };
        inline std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> getFromSocketQueue() { return m_queue_fromSocket; };
        inline void bindOutQueue(std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> queue) { m_queue_toSocket = queue; };
    private:
        void fromLocalThread();
        void fromAirThread();
        void decodeAndSend();
    private:
        std::unique_ptr<std::thread> m_fromLocalThread;
        std::unique_ptr<std::thread> m_fromAirThread;
    private:
        std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> m_queue_in, m_queue_out;
        std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> m_queue_fromSocket, m_queue_toSocket;
        std::shared_ptr<HFDP_Socket> m_sockData;
        uint8_t m_rssiCurr = 255;
        unsigned int m_M, m_N;
        unsigned char **m_dataBlocksIn, **m_fecBlocksIn, **m_dataBlocksOut, **m_fecBlocksOut;
        unsigned int m_M_rx_curr = 0, m_N_rx_curr = 0, m_M_tx_curr = 0, m_N_tx_curr = 0;
    };
}


#endif