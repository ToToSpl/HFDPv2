#ifndef HFDP_HELPER_FUNCS
#define HFDP_HELPER_FUNCS

namespace HFDP::helpers {
    inline void string_to_mac(std::string const& s, char* mac) {
        int last = -1;
        int rc = sscanf(s.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx%n",
                        mac, mac + 1, mac + 2, mac + 3, mac + 4, mac + 5,
                        &last);
    };
}

#endif