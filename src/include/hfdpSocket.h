#ifndef HFDP_SOCKET
#define HFDP_SOCKET

#include <string>
#include "../../lib/json/json.hpp"
#include "helpers.h"

namespace HFDP {
    class HFDP_Socket {
    public:
        HFDP_Socket(nlohmann::json sockConfig);
        ~HFDP_Socket();
    public:
        inline int getID() { return m_ID; };
        inline int getPort() { return m_Port; };
        inline bool getFEC() { return m_FEC; };
        inline unsigned int getFECm() { return m_FEC_m; };
        inline unsigned int getFECn() { return m_FEC_n; };
        inline char* getMAC() { return m_Target_MAC; };
        inline std::string getDirection() { return m_Direction; };
        inline int getBufSize() { return m_BufferSize; };
        inline std::string getSocketType() { return m_SocketType; };
        inline int getResending() { return m_Resending; }; 
    public:
        inline uint8_t getPrevRssi() { return m_prev_RSSI; };
        inline void setPrevRssi(uint8_t rssi) { m_prev_RSSI = rssi; };
    private:
        int m_ID;
        int m_Port;
        bool m_FEC;
        unsigned int m_FEC_m;
        unsigned int m_FEC_n;
        char* m_Target_MAC;
        std::string m_Direction;
        int m_BufferSize;
        std::string m_SocketType;
    private:
        unsigned int m_Resending;
        uint8_t m_prev_RSSI = 255;
    };
}

#endif