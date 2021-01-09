#include "hfdpSocket.h"

#define MAC_SIZE 6

namespace HFDP {
    HFDP_Socket::HFDP_Socket(nlohmann::json sockConfig)
    {
        m_ID = sockConfig["id"];
        m_Port = sockConfig["port"];
        m_FEC = sockConfig["fec"];
        if(m_FEC)
        {
            m_FEC_m = sockConfig["fec_m"];
            m_FEC_n = sockConfig["fec_n"];
        }
        m_Target_MAC = new char[MAC_SIZE];
        helpers::string_to_mac(sockConfig["target_mac"], m_Target_MAC);
        m_Direction = sockConfig["direction"];
        m_BufferSize = sockConfig["buffer_size"];
        m_SocketType = sockConfig["socket_type"];
        if(m_FEC)
            m_Resending = 1; // resending is prohibited with FEC on
        else
            m_Resending = sockConfig["resending"]; 
    };
    
    HFDP_Socket::~HFDP_Socket()
    {
        delete m_Target_MAC;
    }

}