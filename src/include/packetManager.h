#ifndef HFDP_PACKET_MANAGER
#define HFDP_PACKET_MANAGER

#include "buffer.h"
#include "hfdpSocket.h"
#include <vector>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>

namespace HFDP {
    class PacketManager;

    class UdpSocket {
    public:
        UdpSocket(std::shared_ptr<HFDP_Socket> data, std::shared_ptr<PacketManager> manager);
        ~UdpSocket();
    public:
        void startSocket();
        void stopSocket();
    private:
        void sockThread();
    private:
        std::unique_ptr<bool> m_IsRunning;
        std::shared_ptr<HFDP_Socket> m_SockData;
        std::shared_ptr<PacketManager> m_Manager;
        struct sockaddr_in m_Addr;
		int m_socket = 0;
        std::unique_ptr<std::thread> m_SockThread;
        char* m_TempBuf;
    };

    class PacketManager {
        public:
            PacketManager();
            ~PacketManager();
        public:
            void pushFromSocket(std::shared_ptr<HFDP_Socket> sockDat, char* buf, std::size_t size);
            void popToSocket();
        public:
            void pushFromRadio();
            void popToRadio();
        public:
            void addSocket(std::shared_ptr<HFDP_Socket> data, std::shared_ptr<UdpSocket> sock);
        private:
            std::unique_ptr<Buffer::Buffer> m_InBuffer;
            std::unique_ptr<Buffer::Buffer> m_OutBuffer;
            char* m_MAC_host;
            char* m_MAC_target;
            std::vector<std::shared_ptr<HFDP_Socket>> m_socket_datas;
            std::vector<std::shared_ptr<UdpSocket>> m_sockets;
    };
}

#endif