#ifndef HFDP_UDP_SOCKET
#define HFDP_UDP_SOCKET

#include "../../lib/queue/blockingconcurrentqueue.h"
#include "../../lib/loguru/loguru.hpp"

#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#include "hfdpSocket.h"
#include "dataPacket.h"
#include "fecManager.h"


namespace HFDP {
    class UdpSocket {
    public:
        UdpSocket(std::shared_ptr<HFDP_Socket> sock_data);
        ~UdpSocket() { };
    public:
        void startSock();
        void setRxQueue(std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> queue);
        std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> getTxQueue();
    public:
        inline void setID(int id) { m_ID = id; };
        inline int getID() { return m_ID; };
    private:
        void sockInThread();
        void sockOutThread();
    private:
        std::unique_ptr<FecManager> m_fecManager;
    private:
        int m_ID;
        std::shared_ptr<HFDP_Socket> m_Sock_data;
        std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> m_queue_out;
        std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> m_queue_in;
        std::unique_ptr<std::thread> m_Sock_In;
        std::unique_ptr<std::thread> m_Sock_Out;
        struct sockaddr_in m_addr;
        int m_sockfd;
    };
}


#endif