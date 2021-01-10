#include "fecManager.h"
#include "hfdpPacketConsts.h"

namespace HFDP {
    FecManager::FecManager(std::shared_ptr<HFDP_Socket> dataSock) : m_sockData(dataSock)
    {
        m_queue_out = std::make_shared<moodycamel::BlockingConcurrentQueue<DataPacket>>(10);
        m_queue_fromSocket = std::make_shared<moodycamel::BlockingConcurrentQueue<DataPacket>>(10);

        m_M = m_sockData->getFECm();
        m_N = m_sockData->getFECn();
        fec_init();
        
        m_dataBlocksIn = new unsigned char*[m_M];
        m_dataBlocksOut = new unsigned char*[m_M];
        for(unsigned int i = 0; i < m_M; i++)
        {
            m_dataBlocksIn[i] = new unsigned char[m_sockData->getBufSize()];
            m_dataBlocksOut[i] = new unsigned char[m_sockData->getBufSize()];
        }

        m_fecBlocksIn = new unsigned char*[m_N];
        m_fecBlocksOut = new unsigned char*[m_N];
        for(unsigned int i = 0; i < m_N; i++)
        {
            m_fecBlocksIn[i] = new unsigned char[m_sockData->getBufSize()];
            m_fecBlocksOut[i] = new unsigned char[m_sockData->getBufSize()];
        }
    }

    FecManager::~FecManager()
    {
        for(unsigned int i = 0; i < m_M; i++)
        {
            delete m_dataBlocksIn[i];
            delete m_dataBlocksOut[i];
        }
        delete m_dataBlocksIn;
        delete m_dataBlocksOut;

        for(unsigned int i = 0; i < m_N; i++)
        {
            delete m_fecBlocksIn[i];
            delete m_fecBlocksOut[i];
        }
        delete m_fecBlocksIn;
        delete m_fecBlocksOut;
    }

    void FecManager::startFec()
    {
        m_fromAirThread = std::make_unique<std::thread>(&FecManager::fromAirThread, this);
        m_fromLocalThread = std::make_unique<std::thread>(&FecManager::fromLocalThread, this);
    }

    void FecManager::fromAirThread()
    {
        DataPacket recieved;

        while(1)
        {
            m_queue_out->wait_dequeue(recieved);

            if(recieved.rssi != m_rssiCurrRx)
            {
                if(!(m_M_rx_curr == 0 && m_N_rx_curr == 0)) {
                    decodeAndSend();
                    m_M_rx_curr = 0;
                    m_N_rx_curr = 0;
                }
            }
            m_rssiCurrRx = recieved.rssi;

            if(recieved.flags ^ DATA_PACK) {
                std::memcpy(m_dataBlocksIn[m_M_rx_curr], recieved.start, m_sockData->getBufSize());
                delete recieved.start;
                m_M_rx_curr++;
            } else if (recieved.flags ^ FEC_PACK) {
                std::memcpy(m_fecBlocksIn[m_N_rx_curr], recieved.start, m_sockData->getBufSize());
                delete recieved.start;
                m_N_rx_curr++;
            } else {
                m_queue_toSocket->enqueue(recieved);
            }

            if(m_M_rx_curr == m_M && m_N_rx_curr == m_N) {
                decodeAndSend();
            }

        }
    }

    void FecManager::decodeAndSend()
    {
        unsigned int fecNos, erasedBlocks;

        fec_decode(
            (unsigned int) m_sockData->getBufSize(),
            m_dataBlocksIn,
            m_M_rx_curr,
            m_fecBlocksIn,
            &fecNos,
            &erasedBlocks,
            m_N_rx_curr
        );

        for(unsigned int i = 0; i < m_M; i++)
        {
            DataPacket temp;
            temp.start = (char*)m_dataBlocksIn[i];
            temp.size = (std::size_t)m_sockData->getBufSize();
            m_queue_toSocket->enqueue(temp);
        }
    }

    void FecManager::fromLocalThread()
    {
        DataPacket recieved;

        while(1)
        {
            m_queue_fromSocket->enqueue(recieved);

            if(recieved.size != m_sockData->getBufSize()) {
                m_queue_in->enqueue(recieved);
                continue;
            }

            std::memcpy(m_dataBlocksOut[m_M_tx_curr], recieved.start, m_sockData->getBufSize());
            m_M_tx_curr++;

            if(m_M_tx_curr == m_M)
            {
                encodeAndSend();
                m_rssiCurrTx++;
                m_M_tx_curr = 0;
            }

            delete recieved.start;
        }
    }

    void FecManager::encodeAndSend()
    {
        fec_encode(
            (unsigned int) m_sockData->getBufSize(),
            m_dataBlocksOut,
            m_M,
            m_fecBlocksOut,
            m_N
        );

        for(unsigned int i = 0; i < m_M; i++)
        {
            DataPacket temp;
            temp.start = (char*)m_dataBlocksOut[i];
            temp.size = (std::size_t)m_sockData->getBufSize();
            temp.id = m_sockData->getID();
            temp.rssi = m_rssiCurrTx;
            temp.flags = DATA_PACK;
            m_queue_in->enqueue(temp);
        }

        for(unsigned int i = 0; i < m_N; i++)
        {
            DataPacket temp;
            temp.start = (char*)m_fecBlocksOut[i];
            temp.size = (std::size_t)m_sockData->getBufSize();
            temp.id = m_sockData->getID();
            temp.rssi = m_rssiCurrTx;
            temp.flags = FEC_PACK;
            m_queue_in->enqueue(temp);
        }
    }
}