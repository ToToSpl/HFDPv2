#include "packetManager.h"

namespace HFDP {
    PacketManager::PacketManager()
    {
        m_from_air_queue = std::make_shared<moodycamel::BlockingConcurrentQueue<DataPacket>>(20);
        m_from_local_queue = std::make_shared<moodycamel::BlockingConcurrentQueue<DataPacket>>(20);
    }

    inline void PacketManager::bindSocket(std::shared_ptr<UdpSocket> udp, std::shared_ptr<HFDP_Socket> info)
    {
        m_sockets.push_back({udp, info});
    }

    inline void PacketManager::bindPcapDevice(std::shared_ptr<Pcap> device)
    {
        device->bindRecieveQueue(m_from_air_queue);
        device->bindSendQueue(m_from_local_queue);
    }
}