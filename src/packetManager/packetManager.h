#ifndef HFDP_PACKET_MANAGER
#define HFDP_PACKET_MANAGER

namespace HFDP {
    class PacketManager {
        public:
            PacketManager();
            ~PacketManager();
        public:
            pushFromSocket();
            popToSocket();
        public:
            pushFromRadio();
            popToRadio();
    };
}

#endif