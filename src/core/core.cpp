#include "core.h"
#include <fstream>
#include "../../lib/loguru/loguru.hpp"

namespace HFDP {

    Core::Core() {}

    CoreState Core::loadConfig(std::string filepath)
    {
        std::ifstream inConfig(filepath);
        if(!inConfig.is_open())
        {
            LOG_F(ERROR, "Failed to open config file: %s", filepath.c_str());
            return CoreState::Config_Error;
        }

        inConfig >> m_ConfigJson;
        LOG_F(INFO, "Loaded config file: %s", filepath.c_str());
        return CoreState::Config_Success;
    }


}