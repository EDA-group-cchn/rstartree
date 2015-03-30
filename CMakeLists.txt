cmake_minimum_required(VERSION 3.1)
project(rstartree)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(rstartree ${SOURCE_FILES} rstartraits.h boundingbox.h rstartree.h rstarexception.h)