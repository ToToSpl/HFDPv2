#include "pcapDevice.h"
#include <cstring>

static void rx_callback(u_int8_t *user, const struct pcap_pkthdr *h, const u_int8_t *bytes);

namespace HFDP {

    Pcap::Pcap(std::string device, char* mac)
    {
        char error_buffer[PCAP_ERRBUF_SIZE];
        int lookup_return_code;

        m_global_device = pcap_open_live(device.c_str(), 1000, 1, 0, error_buffer);
        if (m_global_device == NULL) {
		    LOG_F(ERROR, "Couldn't open wifi device %s: %s", device.c_str(), error_buffer);
	    }

        lookup_return_code = pcap_setnonblock(m_global_device, 1, error_buffer);
        if(lookup_return_code != 0){
            LOG_F(ERROR, "Error in setting nonblocking mode! %s", error_buffer);
        }

        char szProgram[512];
        struct bpf_program bpfprogram;
        sprintf(szProgram, "wlan addr3 %.2x:%.2x:%.2x:%.2x:%.2x:%.2x and greater %i",
            mac[0] & 0xFF, mac[1] & 0xFF, mac[2] & 0xFF,
            mac[3] & 0xFF, mac[4] & 0xFF, mac[5] & 0xFF,
            (CUT_RADIOTAP_SIZE + IEEE_SIZE));

        if(pcap_compile(m_global_device, &bpfprogram, szProgram, 1, 0) == -1){
            LOG_F(ERROR, "error in compiling pcap script!, %s", szProgram);
        }

        if(pcap_setfilter(m_global_device, &bpfprogram) == -1){
            LOG_F(ERROR, "error in setting filter!");
        }

        pcap_freecode(&bpfprogram);

        LOG_F(INFO, "PCAP init success\n");
    }

    void Pcap::startDevice()
    {
        m_transmit_thread = std::make_unique<std::thread>(&HFDP::Pcap::tx_thread, this);
        m_recieve_thread = std::make_unique<std::thread>(&HFDP::Pcap::rx_thread, this);    
    }

    void Pcap::rx_thread()
    {
        pcap_loop(m_global_device, -1, rx_callback, (u_int8_t*) this);
        LOG_F(ERROR, "Pcap loop broken!");
    }

    void Pcap::tx_thread()
    {
        DataPacket packet;
        int lookup_return_code;

        while(1)
        {
            m_queue_ToSend->wait_dequeue(packet);

            lookup_return_code = pcap_inject(m_global_device, packet.start, packet.size);
            LOG_IF_F(WARNING, (int)packet.size != lookup_return_code,
                "Error during sending! size of packet: %i. Should be: %i", lookup_return_code, (int)packet.size);
            
            delete packet.start;
        }
    }

}

static void rx_callback(u_int8_t *user, const struct pcap_pkthdr *h, const u_int8_t *bytes)
{
    HFDP::Pcap *pdevice = reinterpret_cast<HFDP::Pcap *>(user);

    char* data = new char[h->len];
    std::memcpy(data, bytes, h->len);

    HFDP::DataPacket packet = {0, data, h->len};
    pdevice->getRxQueue()->enqueue(packet);
}
