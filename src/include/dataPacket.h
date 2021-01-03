#ifndef HFDP_DATAPACKET
#define HFDP_DATAPACKET

namespace HFDP {
    struct DataPacket{
        uint8_t id;
        char* start;
        std::size_t size;
    };
}

#endif