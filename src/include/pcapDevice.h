#ifndef HFDP_PCAP
#define HFDP_PCAP

#include <pcap.h>
#include <string>

#include "../../lib/queue/blockingconcurrentqueue.h"
#include "../../lib/loguru/loguru.hpp"

#include "dataPacket.h"
#include "hfdpPacketConsts.h"

namespace HFDP {
    class Pcap {
    public:
        Pcap(std::string device, char* mac);
        ~Pcap() {};
    public:
        void startDevice();
        inline void bindSendQueue(std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> queue) { m_queue_ToSend = queue; };
        inline void bindRecieveQueue(std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> queue) { m_queue_ToRecieve = queue; };
        inline std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> getRxQueue() { return m_queue_ToRecieve; };
    private:
        void rx_thread();
        void tx_thread();
    private:
        std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> m_queue_ToSend;
        std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> m_queue_ToRecieve;
        std::unique_ptr<std::thread> m_transmit_thread;
        std::unique_ptr<std::thread> m_recieve_thread;
        pcap_t *m_global_device;
    };
}

#endif