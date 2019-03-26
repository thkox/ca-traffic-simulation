cmake_minimum_required(VERSION 3.12)
project(ca_traffic_simulation)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_COMPILER /usr/local/bin/g++-8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -O3")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -DDEBUG -Wall")

add_executable(cats src/main.cpp src/Road.cpp src/Road.h src/Lane.cpp src/Lane.h src/Vehicle.cpp src/Vehicle.h src/Simulation.cpp src/Simulation.h src/Inputs.cpp src/Inputs.h)