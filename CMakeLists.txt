cmake_minimum_required(VERSION 3.16)
project(Socket)

set(CMAKE_CXX_STANDARD 11)
include_directories(HardSocket/src)
link_libraries(pthread)

add_executable(Socket HardSocket/src/tcp_server.cpp HardSocket/server.cpp)

