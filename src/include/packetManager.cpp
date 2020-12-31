#include "packetManager.h"

namespace HFDP {
    PacketManager::PacketManager()
    {
        m_InBuffer = std::make_unique<Buffer::Buffer>(HFDP_DEFAULT_BUFFER_SIZE);
        m_OutBuffer = std::make_unique<Buffer::Buffer>(HFDP_DEFAULT_BUFFER_SIZE);
    }

    PacketManager::~PacketManager()
    {

    }

    void PacketManager::addSocket(std::shared_ptr<HFDP_Socket> data, std::shared_ptr<UdpSocket> sock)
    {
        m_sockets.push_back(sock);
        m_socket_datas.push_back(data);
    }
}