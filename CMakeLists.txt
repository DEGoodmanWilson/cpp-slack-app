cmake_minimum_required(VERSION 3.3)
project(cpp_slack_bot)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(cpp_slack_bot ${SOURCE_FILES})