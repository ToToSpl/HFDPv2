#ifndef HFDP_PACKET_MANAGER
#define HFDP_PACKET_MANAGER

#include <memory>
#include <thread>

#include "udpSocket.h"
#include "hfdpSocket.h"
#include "pcapDevice.h"

namespace HFDP {
    class PacketManager {
    public:
        PacketManager();
        ~PacketManager() { };
    public:
        void bindSocket(std::shared_ptr<UdpSocket> udp, std::shared_ptr<HFDP_Socket> info);
        void bindPcapDevice(std::shared_ptr<Pcap> device);
    public:
        void startManager();
        inline void setMacs(std::vector<char*> vec, char* this_mac) { m_mac_map = vec; m_this_mac = this_mac; };
    private:
        void fromAirThread();
        void fromLocalThread();
    private:
        DataPacket generateHFDPpacket(uint8_t id, uint8_t flags, uint8_t rssi, char* reMac, uint16_t size, char* data);
        DataPacket generateHFDPpacket(uint8_t id, uint8_t flags, uint8_t rssi, char* reMac, uint16_t size, char* data, char* ptr);
    private:
        std::unique_ptr<std::thread> m_air_thread;
        std::unique_ptr<std::thread> m_local_thread;
    private:
        std::vector<std::pair<std::shared_ptr<UdpSocket>, std::shared_ptr<HFDP_Socket>>> m_sockets;
        std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> m_from_air_queue;
        std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> m_from_local_queue;
        std::shared_ptr<moodycamel::BlockingConcurrentQueue<DataPacket>> m_local_queue;
    private:
        std::vector<char*> m_mac_map;
        char* m_this_mac;
        uint8_t m_prev_rssi = 255;
        uint8_t m_rssi;
    };
}

#endif