cmake_minimum_required(VERSION 3.20)
project(AudioDetector)

set(CMAKE_CXX_STANDARD 17)

add_executable(AudioDetector main.cpp include/ad_winapi.h src/ad_winapi.cpp)
