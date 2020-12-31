#include "packetManager.h"
#include "../../lib/loguru/loguru.hpp"

namespace HFDP {

    UdpSocket::UdpSocket(std::shared_ptr<HFDP_Socket> data, std::shared_ptr<PacketManager> manager):
		m_SockData(data), m_Manager(manager)
    {
        m_Addr.sin_family = AF_INET;
		m_Addr.sin_port = htons(m_SockData->getPort());
		m_Addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
		m_socket = socket(AF_INET, SOCK_DGRAM, 0);
		if(m_socket < 0)
		{
			LOG_F(ERROR, "Failed to create socket!");
		}

		if(m_SockData->getSocketType() == "server")
		{
			if(bind(m_socket, (const struct sockaddr *)&m_Addr, sizeof(m_socket)) < 0) 
			{ 
				LOG_F(ERROR, "Failed to bind server socket!");
			}
		}

		m_IsRunning = std::make_unique<bool>(false);
		m_TempBuf = new char[m_SockData->getBufSize()];
    }

	UdpSocket::~UdpSocket()
	{
		delete m_TempBuf;
	}

	void UdpSocket::startSocket()
	{
		*m_IsRunning = true;
		m_SockThread = std::make_unique<std::thread>(&UdpSocket::sockThread, this);
	}

	void UdpSocket::sockThread()
	{
		socklen_t len = sizeof(m_Addr);
		std::size_t n = -1;

		while(*m_IsRunning && n == -1)
		{
			n = recvfrom(m_socket, m_TempBuf, m_SockData->getBufSize(), 
						0, ( struct sockaddr *) &m_socket, &len);
			m_Manager->pushFromSocket(m_SockData, m_TempBuf, n);
		}
	}

}