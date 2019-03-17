cmake_minimum_required(VERSION 3.12)
project(ca_traffic_simulation)

set(CMAKE_CXX_STANDARD 17)

add_executable(ca_traffic_simulation src/main.cpp src/Road.cpp src/Road.h)