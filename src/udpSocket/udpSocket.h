#ifndef HFDP_UDPSOCKET
#define HFDP_UDPSOCKET

#include <string>

namespace HFDP {
    enum class UdpSocketType {
        Client,
        Server,
    };

    class UdpSocket {
    public:
        UdpSocket(std::string addr, int port, UdpSocketType type);
    private:
        std::string m_Addr;
        int m_Port;
        UdpSocketType m_Type;
        std::unique_ptr<bool> m_IsRunning;
        
    };
}

#endif