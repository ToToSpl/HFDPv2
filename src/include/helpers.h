#ifndef HFDP_HELPER_FUNCS
#define HFDP_HELPER_FUNCS

#define MAC_SIZE 6

#include <vector>
#include "../../lib/loguru/loguru.hpp"

namespace HFDP::helpers {
    inline void string_to_mac(std::string const& s, char* mac)
    {
        int last = -1;
        int rc = sscanf(s.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx%n",
                        mac, mac + 1, mac + 2, mac + 3, mac + 4, mac + 5,
                        &last);
    };

    inline bool compare_macs(char* mac1, char*mac2)
    {
        for(uint8_t i = 0; i < MAC_SIZE; i++)
        {
            if(mac1[i] != mac2[i])
                return false;
        }
        return true;
    }

    inline int get_mac_index(std::vector<char*> map, char* targetMac)
    {
        int thisInMap = -1;
        for(int i = 0; i < map.size(); i++)
        {
            if(helpers::compare_macs(targetMac, map[i]))
            {
                thisInMap = i;
                break;
            }
        }
        return thisInMap;
    }

    inline void hex_log(char* data, std::size_t len)
    {
        char* formatted = new char[len+1];
        for(std::size_t i = 0; i < len; i++)
        {
            sprintf(formatted + i, "%X ", data[i]);
        }
        formatted[len] = '\n';
        LOG_F(INFO, formatted);
        delete formatted;
    }
}

#endif