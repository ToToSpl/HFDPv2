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
        inline char* getMAC() { return m_Target_MAC; };
        inline std::string getDirection() { return m_Direction; };
        inline int getBufSize() { return m_BufferSize; };
        inline std::string getSocketType() { return m_SocketType; };
        inline int getResending() { return m_Resending; }; 
    private:
        int m_ID;
        int m_Port;
        bool m_FEC;
        char* m_Target_MAC;
        std::string m_Direction;
        int m_BufferSize;
        std::string m_SocketType;
        int m_Resending;
    };
}

#endif