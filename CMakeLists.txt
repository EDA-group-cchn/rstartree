cmake_minimum_required(VERSION 3.1)
project(rstartree)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp)
add_executable(rstartree ${SOURCE_FILES} src/rstartraits.h src/boundingbox.h src/rstartree.h src/rstarexception.h)