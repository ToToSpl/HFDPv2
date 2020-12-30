cmake_minimum_required(VERSION 3.12)

project(HFDP)

set(CMAKE_CXX_FLAGS "-std=c++17 -Werror")

add_executable(HFDP
    lib/loguru/loguru.cpp
    lib/fec/fec.c
    src/main.cpp
    src/core/core.cpp
    src/udpSocket/udpSocket.cpp
    src/buffer/buffer.cpp
    src/packetManager/packetManager.cpp
)

target_link_libraries(HFDP
	-lpcap
    -pthread
    -ldl
)