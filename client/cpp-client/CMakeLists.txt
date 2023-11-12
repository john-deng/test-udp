cmake_minimum_required(VERSION 3.0)
project(TCP_UDP_Client)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(client main.cpp)

target_link_libraries(client pthread)
