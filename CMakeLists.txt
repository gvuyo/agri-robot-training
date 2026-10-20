cmake_minimum_required(VERSION 3.10)
project(agri-robot-training VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(include)
add_executable(hello_cxx src/hello_cxx.cpp)


