cmake_minimum_required(VERSION 2.8.12)

project(HFDP)

if(NOT MSVC)
	add_definitions("-std=c++17 -Wall")
else()
	add_definitions("-std=c++17 -WX -W2")
endif()

add_executable(HFDP
    lib/loguru/loguru.cpp
    src/main.cpp
)

target_link_libraries(HFDP
	-lpcap
    -pthread
    -ldl
)