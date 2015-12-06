cmake_minimum_required(VERSION 3.3)
project(redstone)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#deps
add_subdirectory(engine)

add_subdirectory(mongoose-cpp)

set(SQLITECPP_RUN_CPPLINT OFF)
set(SQLITECPP_RUN_CPPCHECK OFF)
add_subdirectory(SQLiteCpp)

set(SOURCE_FILES main.cpp app_web_controller.cpp app_web_controller.h app_persistent_store.cpp app_persistent_store.h)

add_executable(redstone ${SOURCE_FILES})

include_directories(SYSTEM
        engine/include
        engine/cpr/include
        mongoose-cpp
        SQLiteCpp/include
        engine/jsoncpp #we're going to borrow the JSON stuff!
        )
link_directories(
        ${SLACK_LIBRARY_DIR}
)

target_link_libraries(redstone mongoose SQLiteCpp sqlite3 engine cpr)