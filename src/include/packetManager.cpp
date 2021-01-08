#include "packetManager.h"

namespace HFDP {
    PacketManager::PacketManager()
    {
        m_from_air_queue = std::make_shared<moodycamel::BlockingConcurrentQueue<DataPacket>>(20);
        m_from_local_queue = std::make_shared<moodycamel::BlockingConcurrentQueue<DataPacket>>(20);
        m_local_queue = std::make_shared<moodycamel::BlockingConcurrentQueue<DataPacket>>(20);
    }

    void PacketManager::bindSocket(std::shared_ptr<UdpSocket> udp, std::shared_ptr<HFDP_Socket> info)
    {
        m_sockets.push_back({udp, info});
        udp->setRxQueue(m_local_queue);
    }

    void PacketManager::bindPcapDevice(std::shared_ptr<Pcap> device)
    {
        device->bindRecieveQueue(m_from_air_queue);
        device->bindSendQueue(m_from_local_queue);
    }


    void PacketManager::startManager()
    {
        m_local_thread = std::make_unique<std::thread>(&PacketManager::fromLocalThread, this);
        m_air_thread = std::make_unique<std::thread>(&PacketManager::fromAirThread, this);
    }

    void PacketManager::fromAirThread()
    {
        DataPacket tempPack;
        uint8_t *id = new uint8_t[ID_SIZE];
        uint8_t *flags = new uint8_t[FLAGS_SIZE];
        uint8_t *rssi = new uint8_t[RSSI_SIZE];
        uint16_t size = 0;
        char *reMAC = new char[MAC_SIZE];

        while(1)
        {
            // wait for packet
            m_from_air_queue->wait_dequeue(tempPack);
            // get data from HFDP header
            std::memcpy(id, (uint8_t*)tempPack.start + HFDP_START_PLACE + ID_OFFSET, ID_SIZE);
            std::memcpy(flags, (uint8_t*)tempPack.start + HFDP_START_PLACE + FLAGS_OFFSET, FLAGS_SIZE);
            std::memcpy(rssi, (uint8_t*)tempPack.start + HFDP_START_PLACE + RSSI_OFFSET, RSSI_SIZE);
            //helpers::hex_log(tempPack.start + HFDP_START_PLACE + SIZE_OFFSET, tempPack.size - HFDP_START_PLACE - SIZE_OFFSET);
            {
                uint8_t size1 = (uint16_t)tempPack.start[HFDP_START_PLACE + SIZE_OFFSET];
                uint8_t size2 = (uint8_t)tempPack.start[HFDP_START_PLACE + SIZE_OFFSET + 1];
                size = (uint16_t)size2 << 8 | (uint16_t)size1;
            }
            // TODO: write resend
            // check if this packet should be resend 
            // if(*flags & RESEND)
            // {
            //     std::memcpy(reMAC, tempPack.start + HFDP_START_PLACE + REMAC_OFFSET, REMAC_SIZE);
            //     int index = helpers::get_mac_index(m_mac_map, reMAC);
            //     if(index < 0)
            //     {
            //         delete tempPack.start;
            //         continue;
            //     }
            //     int thisIndex = helpers::get_mac_index(m_mac_map, m_this_mac);
            //     char* targetMAC = new char[MAC_SIZE];
            //     if(std::abs(index - thisIndex) == 1)
            //         *flags ^= RESEND;
            //     if(index > thisIndex)
            //         targetMAC = m_mac_map[thisIndex + 1];
            //     else
            //         targetMAC = m_mac_map[thisIndex - 1];
            //     DataPacket newPack = generateHFDPpacket(*id, *flags, *rssi, reMAC, *size, tempPack.start + HEADER_SIZE, tempPack.start + HEADER_SIZE);
            //     std::memcpy(tempPack.start + )
            //     m_from_local_queue->enqueue(newPack);
            //     delete targetMAC;
            //     continue;
            // }
            // TODO: Write FRACTURED_PACKET system
            for(auto sockPair : m_sockets)
            {
                if((uint8_t)sockPair.second->getID() == *id)
                {
                    DataPacket toSend;
                    toSend.id = *id;
                    toSend.size = size;
                    
                    toSend.start = new char[size];
                    std::memcpy(toSend.start, tempPack.start + HFDP_START_PLACE + DATA_OFFSET, size);
                    delete tempPack.start;
                    sockPair.first->getTxQueue()->enqueue(toSend);
                    break;
                }
            }
        }
    }

    void PacketManager::fromLocalThread()
    {
        DataPacket tempPack;
        uint8_t flags = 0;

        while(1)
        {
            m_local_queue->wait_dequeue(tempPack);
            for(auto sockPair : m_sockets)
            {
                if((uint8_t)sockPair.second->getID() == tempPack.id)
                {
                    DataPacket toSend;
                    toSend.size = IEEE_SIZE + RADIOTAP_SIZE + HEADER_SIZE + tempPack.size;
                    toSend.start = new char[toSend.size];
                    std::memcpy(toSend.start, &u8aRadiotapHeader, RADIOTAP_SIZE);
                    std::memcpy(toSend.start + RADIOTAP_SIZE, &u8aIeeeHeader_beacon, IEEE_SIZE);
                    std::memcpy(toSend.start + RADIOTAP_SIZE + MAC_OFFSET, m_this_mac, MAC_SIZE);
                    std::memcpy(toSend.start + RADIOTAP_SIZE + MAC_OFFSET + MAC_SIZE, sockPair.second->getMAC(), MAC_SIZE);
                    generateHFDPpacket(tempPack.id, flags, ++m_rssi, nullptr, tempPack.size, tempPack.start, toSend.start + RADIOTAP_SIZE + IEEE_SIZE);
                    m_from_local_queue->enqueue(toSend);
                    break;
                }
            }
            delete tempPack.start;
        }
    }

    DataPacket PacketManager::generateHFDPpacket(uint8_t id, uint8_t flags, uint8_t rssi, char* reMac, uint16_t size, char* data)
    {
        char* buffer = new char[size + HEADER_SIZE];
        std::memcpy(buffer + ID_OFFSET, &id, ID_SIZE);
        std::memcpy(buffer + FLAGS_OFFSET, &flags, FLAGS_SIZE);
        std::memcpy(buffer + RSSI_OFFSET, &rssi, RSSI_SIZE);
        if(reMac == nullptr)
            std::memset(buffer + REMAC_OFFSET, 0, REMAC_SIZE);
        else
            std::memcpy(buffer + REMAC_OFFSET, reMac, REMAC_SIZE);
        uint16_t size1 = size & 0x00FF;   
        uint16_t size2 = size >> 8;
        std::memcpy(buffer + SIZE_OFFSET, &size1, 1);
        std::memcpy(buffer + SIZE_OFFSET+1, &size2, 1);
        std::memcpy(buffer + DATA_OFFSET, data, size);
        return {id, buffer, ((std::size_t)size + HEADER_SIZE)};
    }

    DataPacket PacketManager::generateHFDPpacket(uint8_t id, uint8_t flags, uint8_t rssi, char* reMac, uint16_t size, char* data, char* ptr)
    {
        std::memcpy(ptr + ID_OFFSET, &id, ID_SIZE);
        std::memcpy(ptr + FLAGS_OFFSET, &flags, FLAGS_SIZE);
        std::memcpy(ptr + RSSI_OFFSET, &rssi, RSSI_SIZE);
        if(reMac == nullptr)
            std::memset(ptr + REMAC_OFFSET, 0, REMAC_SIZE);
        else
            std::memcpy(ptr + REMAC_OFFSET, reMac, REMAC_SIZE);
        uint16_t size1 = size & 0x00FF;   
        uint16_t size2 = size >> 8;
        std::memcpy(ptr + SIZE_OFFSET, &size1, 1);
        std::memcpy(ptr + SIZE_OFFSET+1, &size2, 1);
        std::memcpy(ptr + DATA_OFFSET, data, size);
        return {id, ptr, (std::size_t)(size + HEADER_SIZE)};
    }

}