#include "udpSocket.h"


namespace HFDP {

    UdpSocket::UdpSocket(std::shared_ptr<HFDP_Socket> sock_data): m_Sock_data(sock_data)
    {

        //m_queue_in = std::make_shared<moodycamel::BlockingConcurrentQueue<DataPacket>>(10);
        m_queue_out = std::make_shared<moodycamel::BlockingConcurrentQueue<DataPacket>>(10);

        m_ID = m_Sock_data->getID();

        if((m_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            LOG_F(ERROR, "Couldn't create socket: %d", sock_data->getPort());
        }

        std::memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sin_family = AF_INET;
        m_addr.sin_addr.s_addr = INADDR_ANY ; //everyone on the net can use the system !!!!
        m_addr.sin_port = htons(m_Sock_data->getPort());

        if(m_Sock_data->getSocketType() == "server")
        {
            if(bind(m_sockfd, (const struct sockaddr *) &m_addr, sizeof(m_addr)) < 0)
            {
                LOG_F(ERROR, "Couldn't bind socket: %d", sock_data->getPort());
            }
        }
    }

    void UdpSocket::startSock()
    {
        m_Sock_In = std::make_unique<std::thread>(&UdpSocket::sockInThread, this);
        m_Sock_Out = std::make_unique<std::thread>(&UdpSocket::sockOutThread, this);
    }

    void UdpSocket::sockInThread()
    {

        socklen_t len = sizeof(m_addr);
		std::size_t msg_len = 0;
        int buf_size = m_Sock_data->getBufSize();

        while(1)
        {
            char* msg_buf = new char[buf_size];
            msg_len = recvfrom(m_sockfd, msg_buf, buf_size, MSG_WAITALL, (struct sockaddr *) &m_addr, &len);
			if(msg_len <= 0) {
				LOG_F(WARNING, "message in error: %d", m_Sock_data->getPort());
				continue;
			}

            DataPacket pack = {(u_int8_t)m_Sock_data->getID(), msg_buf, msg_len};
            m_queue_in->enqueue(pack);
        }
    }

    void UdpSocket::sockOutThread()
    {
        socklen_t len = sizeof(m_addr);
        DataPacket curPack;

        while(1)
        {
            m_queue_out->wait_dequeue(curPack);
            sendto(m_sockfd, curPack.start, curPack.size, 0, (struct sockaddr *) &m_addr, len);
            delete curPack.start;
        }
    }

}