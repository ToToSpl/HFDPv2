#include "fecManager.h"
#include "hfdpPacketConsts.h"

// returns index of the first placement which not equals to 255
static inline unsigned int firstEmptyIndex(unsigned int* arr, unsigned int arrSize)
{
    for(unsigned int i = 0; i < arrSize; i++)
    {
        if(arr[i] == 255)
            return i;
    }
    return 255;
}

namespace HFDP {
    FecManager::FecManager(std::shared_ptr<HFDP_Socket> dataSock) : m_sockData(dataSock)
    {
        LOG_F(INFO, "creating fec handler for %i", m_sockData->getID());
        m_queue_out = std::make_shared<moodycamel::BlockingConcurrentQueue<DataPacket>>(10);
        m_queue_fromSocket = std::make_shared<moodycamel::BlockingConcurrentQueue<DataPacket>>(10);

        m_N = m_sockData->getFECn();
        m_K = m_sockData->getFECk();
        LOG_F(INFO, "fec id: %i, k: %i, n: %i", m_sockData->getID(), m_K, m_N);
        m_fec_ptr = fec_new((unsigned short) m_K, (unsigned short) m_N);
        
        m_dataBlocksIn = new unsigned char*[m_N];
        m_dataBlocksOut = new unsigned char*[m_N];
        for(unsigned int i = 0; i < m_N; i++)
        {
            m_dataBlocksIn[i] = new unsigned char[m_sockData->getBufSize()];
            m_dataBlocksOut[i] = new unsigned char[m_sockData->getBufSize()];
        }
    }

    FecManager::~FecManager()
    {
        for(unsigned int i = 0; i < m_N; i++)
        {
            delete m_dataBlocksIn[i];
            delete m_dataBlocksOut[i];
        }
        delete m_dataBlocksIn;
        delete m_dataBlocksOut;

        fec_free(m_fec_ptr);
    }

    void FecManager::startFec()
    {
        m_fromAirThread = std::make_unique<std::thread>(&FecManager::fromAirThread, this);
        m_fromLocalThread = std::make_unique<std::thread>(&FecManager::fromLocalThread, this);
    }

    void FecManager::fromAirThread()
    {
        DataPacket recieved;
        unsigned int* writtenIndexes = new unsigned int[m_K];
        std::memset(writtenIndexes, 255, m_K);

        while(1)
        {
            m_queue_out->wait_dequeue(recieved);

            if(recieved.size != m_sockData->getBufSize())
            {
                m_queue_toSocket->enqueue(recieved);
                continue;
            }

            // new chunk, reset sys
            if(recieved.rssi != m_rssiCurrRx)
            {
                m_N_rx_curr = 0;
                m_K_rx_curr = 0;
                m_rssiCurrRx = recieved.rssi;
                std::memset(writtenIndexes, 255, m_K);
            }

            if(m_N_rx_curr == m_K)
            {
                delete recieved.start;
                continue;
            }

            unsigned short fec_index = recieved.flags & FEC_INDEX_MASK;
            if(fec_index < m_K) {
                writtenIndexes[fec_index] = fec_index;
                std::memcpy(m_dataBlocksIn[fec_index], recieved.start, recieved.size);
                m_K_rx_curr++;
            } else {
                auto index = firstEmptyIndex(writtenIndexes, m_K);
                writtenIndexes[index] = fec_index;
                std::memcpy(m_dataBlocksIn[index], recieved.start, recieved.size);
            }
            m_N_rx_curr++;
            delete recieved.start;
            
            if(m_N_rx_curr == m_K)
            {
                fec_decode(m_fec_ptr, (const gf**)m_dataBlocksIn, (gf**)(m_dataBlocksIn + m_K), writtenIndexes, m_sockData->getBufSize());
                unsigned int compIndex = 0;
                for(unsigned int i = 0; i < m_K; i++)
                {
                    char* start = new char[m_sockData->getBufSize()];
                    if(writtenIndexes[i] == i) {
                        std::memcpy(start, m_dataBlocksIn[i], m_sockData->getBufSize());
                    } else {
                        std::memcpy(start, (m_dataBlocksIn + m_K)[compIndex], m_sockData->getBufSize());
                        compIndex++;
                    }

                    DataPacket temp;
                    temp.start = (char*)start;
                    temp.size = m_sockData->getBufSize();
                    m_queue_toSocket->enqueue(temp);
                }
            }
        }
    }

    void FecManager::fromLocalThread()
    {
        DataPacket recieved;
        uint8_t fec_number = 0;

        while(1)
        {
            m_queue_fromSocket->wait_dequeue(recieved);

            if(recieved.size != m_sockData->getBufSize())
            {
                recieved.flags = 0;
                recieved.id = m_sockData->getID();
                recieved.rssi = m_rssiCurrTx;
                m_queue_in->enqueue(recieved);
                continue;
            }

            std::memcpy(m_dataBlocksOut[m_K_tx_curr], recieved.start, m_sockData->getBufSize());
            recieved.flags = fec_number;
            recieved.id = m_sockData->getID();
            recieved.rssi = m_rssiCurrTx;

            m_queue_in->enqueue(recieved);
            m_K_tx_curr++;
            fec_number++;

            if(m_K_tx_curr == m_K)
            {
                fec_encode(m_fec_ptr, (const gf**)m_dataBlocksOut, (gf**)(m_dataBlocksOut + m_K), m_sockData->getBufSize());

                for(unsigned int i = 0; i < (m_N - m_K); i++)
                {
                    DataPacket temp;
                    temp.id = m_sockData->getID();
                    temp.rssi = m_rssiCurrTx;
                    temp.flags = fec_number;
                    temp.size = m_sockData->getBufSize();
                    temp.start = new char[m_sockData->getBufSize()];
                    std::memcpy(temp.start, (m_dataBlocksOut + m_K)[i], m_sockData->getBufSize());
                    m_queue_in->enqueue(temp);
                    fec_number++;
                }

                fec_number = 0;
                m_K_tx_curr = 0;
                m_rssiCurrTx++;
            }
        }
    }
}