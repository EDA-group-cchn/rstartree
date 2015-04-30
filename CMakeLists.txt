cmake_minimum_required(VERSION 2.8.9)
project(rstartree)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(rstartree src/main.cpp src/rstartraits.h src/boundingbox.h src/rstartree.h src/rstarexception.h)
add_executable(client src/client.cpp src/command.cpp src/command.h)
