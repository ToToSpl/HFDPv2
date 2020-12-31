cmake_minimum_required(VERSION 3.12)

project(HFDP)

set(CMAKE_CXX_FLAGS "-std=c++17 -Werror")

add_executable(HFDP
    lib/loguru/loguru.cpp
    lib/hfdp/hfdp.c
    src/main.cpp
    src/include/core.cpp
    src/include/udpSocket.cpp
    src/include/buffer.cpp
    src/include/packetManager.cpp
    src/include/hfdpSocket.cpp
)

target_link_libraries(HFDP
	-lpcap
    -pthread
    -ldl
)