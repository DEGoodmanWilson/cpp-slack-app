cmake_minimum_required(VERSION 3.3)
project(cpp_slack_bot)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#deps
add_subdirectory(cpp-slack-client)

set(USE_SYSTEM_CURL YES)
set(BUILD_CPR_TESTS NO)
add_subdirectory(cpr)
include_directories(${CPR_INCLUDE_DIRS})

add_subdirectory(mongoose-cpp)

set(SOURCE_FILES main.cpp app_web_controller.cpp app_web_controller.h)

add_executable(cpp_slack_app ${SOURCE_FILES})

include_directories(SYSTEM
        ${SLACK_INCLUDE_DIR}
        mongoose-cpp
        )
link_directories(
        ${SLACK_LIBRARY_DIR}
        ${PROJECT_BINARY_DIR}/mongoose
)

target_link_libraries(cpp_slack_app mongoose cpr slack)