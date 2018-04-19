cmake_minimum_required(VERSION 2.8)
project(rw)

add_compile_options(-std=c++11)
SET(CMAKE_CXX_COMPILER g++)
SET(CMAKE_CXX_FLAGS -O3)

add_executable(rw main.cpp Node.cpp Node.h Cluster.cpp Cluster.h common.h SparseMatrix.h)
