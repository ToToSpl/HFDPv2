#include "hfdpSocket.h"

#define MAC_SIZE 6

namespace HFDP {
    HFDP_Socket::HFDP_Socket(nlohmann::json sockConfig)
    {
        m_ID = sockConfig["id"];
        m_Port = sockConfig["port"];
        m_FEC = sockConfig["fec"];
        m_Target_MAC = new char[MAC_SIZE];
        string_to_mac(sockConfig["target_mac"], m_Target_MAC);
        m_Direction = sockConfig["direction"];
        m_BufferSize = sockConfig["buffer_size"];
        m_SocketType = sockConfig["socket_type"];
        m_Resending = sockConfig["resending"];
    };
    
    HFDP_Socket::~HFDP_Socket()
    {
        delete m_Target_MAC;
    }

    inline void HFDP_Socket::string_to_mac(std::string const& s, char* mac) {
        int last = -1;
        int rc = sscanf(s.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx%n",
                        mac, mac + 1, mac + 2, mac + 3, mac + 4, mac + 5,
                        &last);
    };


}