cmake_minimum_required(VERSION 3.0)
project(test_udp)

set(CMAKE_CXX_STANDARD 14)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(test_udp main.cpp)
target_link_libraries(test_udp PRIVATE Threads::Threads)