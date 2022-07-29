cmake_minimum_required(VERSION 3.7)

add_definitions(-std=c++17)

set(CXX_FLAGS "-Wall -Wmultichar")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

project(CapstoneNN)

add_executable(CapstoneNN src/NeuralNetwork.cpp src/main.cpp)
