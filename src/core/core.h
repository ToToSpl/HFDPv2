#ifndef HFDP_CORE
#define HFDP_CORE

#include <string>
#include "../../lib/json/json.hpp"

namespace HFDP {

    enum class CoreState{
        Success,
        Config_Error,
        Config_Success,
        PCAP_Error,
        UDP_Error,
        Buffer_Error,
        Close_Success,
    };

    class Core {
    public:
        Core();
        ~Core();
    public:
        CoreState loadConfig(std::string filepath);
        CoreState startSystem();
        CoreState stopSystem();
    private:
        CoreState bindPcapDevice(std::string deviceName);
        CoreState bindUDP();
        CoreState unBindUDP();
    private:
        nlohmann::json m_ConfigJson;

    };
}


#endif