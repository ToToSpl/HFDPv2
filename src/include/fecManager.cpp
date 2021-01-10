#include "fecManager.h"
#include "../../lib/fec/fec.h"

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
            if(recieved.rssi != m_rssiCurr)
            {
                if(m_M_rx_curr == 0 && m_N_rx_curr == 0) {
                    m_rssiCurr = recieved.rssi;
                } else {
                    decodeAndSend();
                    m_M_rx_curr = 0;
                    m_N_rx_curr = 0;
                    m_rssiCurr = recieved.rssi;
                }
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
        
    }
}