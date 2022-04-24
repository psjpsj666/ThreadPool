cmake_minimum_required(VERSION 3.10)
project(threadpool)
set(CMAKE_CXX_STANDARD 11)
add_executable(threadpool threadpool.h threadpool.cpp test.cpp)