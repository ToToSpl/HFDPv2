#ifndef HFDP_PACKET_MANAGER
#define HFDP_PACKET_MANAGER

#include <memory>
#include <thread>

#include "udpSocket.h"
#include "hfdpSocket.h"
#include "pcapDevice.h"

namespace HFDP {
    class PacketManager {
    public:
        PacketManager();
        ~PacketManager() { };
    public:
        inline void bindSocket(std::shared_ptr<UdpSocket> udp, std::shared_ptr<HFDP_Socket> info);
        inline void bindPcapDevice(std::shared_ptr<Pcap> device);
    public:
        void startManager();
    private:
        void fromAirThread();
        void fromLocalThread();
    private:
        std::unique_ptr<std::thread> m_air_thread;
        std::unique_ptr<std::thread> m_local_thread;
    private:
        std::vector<std::pair<std::shared_ptr<UdpSocket>, std::shared_ptr<HFDP_Socket>>> m_sockets;
        std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> m_from_air_queue;
        std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> m_from_local_queue;
    };
}

#endif