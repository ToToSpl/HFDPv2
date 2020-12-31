#include "core.h"
#include <fstream>
#include "../../lib/loguru/loguru.hpp"

namespace HFDP {

    Core::Core()
    {

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
        string_to_mac(m_ConfigJson["HFDP"]["device_mac"], m_Device_MAC);

        for(auto mac : m_ConfigJson["HFDP"]["MAC_ADDRESSES"])
        {
            char* temp = new char[MAC_SIZE];
            string_to_mac(mac, temp);
            m_MAC_map.push_back(temp);
        }

        m_Manager = std::make_shared<PacketManager>();

        for(auto sock : m_ConfigJson["HFDP"]["HFDP_sockets"])
        {
            auto tempSockData = std::make_shared<HFDP_Socket>(sock);
            auto tempSock = std::make_shared<UdpSocket>(tempSockData, m_Manager);
            m_Manager->addSocket(tempSockData, tempSock);
            m_sockets.push_back(tempSock);
            m_hfdp_sockets.push_back(tempSockData);
        }

        return CoreState::Success;

    }

    CoreState Core::stopSystem()
    {
        LOG_F(INFO, "Stopping system...");

        return CoreState::Success;

    }

    CoreState Core::bindUDP()
    {
        for(auto udpSock : m_sockets)
            udpSock->startSocket();
        
        return CoreState::Success;
    }

    void Core::string_to_mac(std::string const& s, char* mac) {
        int last = -1;
        int rc = sscanf(s.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx%n",
                        mac, mac + 1, mac + 2, mac + 3, mac + 4, mac + 5,
                        &last);
        if(rc != 6 || s.size() != last)
            LOG_F(ERROR, "Wrong MAC format!");
    }


}