cmake_minimum_required(VERSION 2.8)
project(rw)

set(CMAKE_CXX_STANDARD 11)

add_executable(rw main.cpp Node.cpp Node.h Cluster.cpp Cluster.h common.h)