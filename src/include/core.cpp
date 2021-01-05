#include "core.h"
#include <fstream>

namespace HFDP {

    Core::Core()
    {
        // m_queue_ptr = std::make_shared<moodycamel::BlockingConcurrentQueue<DataPacket>>(10);
        m_manager = std::make_unique<PacketManager>();
    }

    Core::~Core()
    {
        delete m_Device_MAC;
        for(auto mac : m_MAC_map)
        {
            delete mac;
        }
    }

    CoreState Core::loadConfig(std::string filepath)
    {
        std::ifstream inConfig(filepath);
        if(!inConfig.is_open())
        {
            LOG_F(ERROR, "Failed to open config file: %s", filepath.c_str());
            return CoreState::Config_Error;
        }

        m_ConfigJson = nlohmann::json::parse(inConfig);
        LOG_F(INFO, "Loaded config file: %s", filepath.c_str());
        return CoreState::Config_Success;
    }

    CoreState Core::startSystem()
    {
        LOG_F(INFO, "Reading config file...");
        
        m_Device_MAC = new char[MAC_SIZE];
        helpers::string_to_mac(m_ConfigJson["HFDP"]["device_mac"], m_Device_MAC);

        for(auto mac : m_ConfigJson["HFDP"]["MAC_ADDRESSES"])
        {
            char* temp = new char[MAC_SIZE];
            helpers::string_to_mac(mac, temp);
            m_MAC_map.push_back(temp);
        }

        for(auto sock : m_ConfigJson["HFDP"]["HFDP_sockets"])
        {
            auto tempSockData = std::make_shared<HFDP_Socket>(sock);
            auto tempSock = std::make_shared<UdpSocket>(tempSockData);
            m_manager->bindSocket(tempSock, tempSockData);
            m_sockets.push_back(tempSock);
            m_hfdp_sockets.push_back(tempSockData);
        }

        m_pcap_device = std::make_shared<Pcap>(m_ConfigJson["HFDP"]["wifi_name"], m_Device_MAC);
        m_manager->bindPcapDevice(m_pcap_device);

        bindManager();
        bindPcap();
        bindUDP();

        return CoreState::Success;

    }

    CoreState Core::bindUDP()
    {
        LOG_F(INFO, "Binding sockets...");
        for(auto udpSock : m_sockets)
            udpSock->startSock();
        
        return CoreState::Success;
    }

    CoreState Core::bindPcap()
    {
        m_pcap_device->startDevice();
        return CoreState::Success;
    }

    CoreState Core::bindManager()
    {
        m_manager->setMacs(m_MAC_map, m_Device_MAC);
        m_manager->startManager();
        return CoreState::Success;
    }
}