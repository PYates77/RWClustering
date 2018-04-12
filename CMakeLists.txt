cmake_minimum_required(VERSION 2.8)
project(rw)

add_compile_options(-std=c++11)
SET(CMAKE_CXX_COMPILER g++)

add_executable(rw main.cpp Node.cpp Node.h Cluster.cpp Cluster.h common.h)
