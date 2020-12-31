#ifndef HFDP_CORE
#define HFDP_CORE

#include <string>
#include "../../lib/json/json.hpp"
#include "packetManager.h"
#include "hfdpSocket.h"

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
        CoreState stopSystem();
    private:
        CoreState bindPcapDevice(std::string deviceName);
        CoreState bindUDP();
        CoreState unBindUDP();
    private:
        void string_to_mac(std::string const& s, char* mac);
    private:
        nlohmann::json m_ConfigJson;
        char* m_Device_MAC;
        std::vector<char*> m_MAC_map;
    private:
        std::shared_ptr<PacketManager> m_Manager;
        std::vector<std::shared_ptr<UdpSocket>> m_sockets;
        std::vector<std::shared_ptr<HFDP_Socket>> m_hfdp_sockets;


    };
}


#endif