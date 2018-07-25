cmake_minimum_required(VERSION 3.5)
project(QrCodeScanner)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp scanner.cpp)
add_executable(QrCodeScanner ${SOURCE_FILES})