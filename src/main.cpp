#include "main.h"
#include "../lib/loguru/loguru.hpp"
#include "include/core.h"
#include <iostream>

int main(int argc, char *argv[])
{
    loguru::init(argc, argv);
    loguru::add_file("everything.log", loguru::Append, loguru::Verbosity_MAX);

    if(argc != 2) {
        LOG_F(ERROR, "Wrong parameters!");
        LOG_F(INFO, USAGE);
        return 1;
    }

    LOG_F(INFO, "HFDP started!");

    HFDP::Core core;
    core.loadConfig(argv[1]);
    core.startSystem();

    LOG_F(WARNING, "Type enter to stop program");
    std::cin.get();

    return 0;
}