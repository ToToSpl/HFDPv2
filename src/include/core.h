#ifndef HFDP_CORE
#define HFDP_CORE

#include <string>
#include "../../lib/json/json.hpp"
#include "../../lib/queue/blockingconcurrentqueue.h"
#include "../../lib/loguru/loguru.hpp"

#include "udpSocket.h"
#include "hfdpSocket.h"
#include "dataPacket.h"
#include "helpers.h"
#include "packetManager.h"

#define MAC_SIZE 6

namespace HFDP {

    enum class CoreState{
        Success,
        Config_Error,
        Config_Success,
        PCAP_Error,
        UDP_Error,
        Buffer_Error,
        Close_Success,
    };

    class Core {
    public:
        Core();
        ~Core();
    public:
        CoreState loadConfig(std::string filepath);
        CoreState startSystem();
    private:
        CoreState bindPcapDevice(std::string deviceName);
        CoreState bindUDP();
        CoreState bindPcap();
        CoreState bindManager();
        
    private:
        nlohmann::json m_ConfigJson;
        char* m_Device_MAC;
        std::vector<char*> m_MAC_map;
    private:
        std::vector<std::shared_ptr<HFDP_Socket>> m_hfdp_sockets;
        std::vector<std::shared_ptr<UdpSocket>> m_sockets;
        std::shared_ptr<Pcap> m_pcap_device;
        std::unique_ptr<PacketManager> m_manager;

    };
}


#endif