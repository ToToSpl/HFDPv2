cmake_minimum_required(VERSION 3.10)

project(HFDP)

set(CMAKE_CXX_FLAGS "-std=c++17 -Werror")

add_executable(HFDP
    lib/loguru/loguru.cpp
    src/main.cpp
    src/include/core.cpp
    src/include/hfdpSocket.cpp
    src/include/udpSocket.cpp
    src/include/pcapDevice.cpp
    src/include/packetManager.cpp
)

target_link_libraries(HFDP
	-lpcap
    -pthread
    -ldl
)