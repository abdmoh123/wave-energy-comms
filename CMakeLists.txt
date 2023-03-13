cmake_minimum_required(VERSION 3.24)
project(transmission_interference_simulator)

set(CMAKE_CXX_STANDARD 17)

add_executable(transmission_interference_simulator main.cpp interference_simulator.cpp)
