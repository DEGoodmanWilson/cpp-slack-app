cmake_minimum_required(VERSION 3.3)
project(redstone)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

#------------------------------------------------------------------------------
# Required CPM Setup - no need to modify - See: https://github.com/iauns/cpm
#------------------------------------------------------------------------------
set(CPM_DIR "${CMAKE_CURRENT_BINARY_DIR}/cpm_packages" CACHE TYPE STRING)
find_package(Git)
if(NOT GIT_FOUND)
    message(FATAL_ERROR "CPM requires Git.")
endif()
if (NOT EXISTS ${CPM_DIR}/CPM.cmake)
    message(STATUS "Cloning repo (https://github.com/iauns/cpm)")
    execute_process(
            COMMAND "${GIT_EXECUTABLE}" clone https://github.com/iauns/cpm ${CPM_DIR}
            RESULT_VARIABLE error_code
            OUTPUT_QUIET ERROR_QUIET)
    if(error_code)
        message(FATAL_ERROR "CPM failed to get the hash for HEAD")
    endif()
endif()
include(${CPM_DIR}/CPM.cmake)

#------------------------------------------------------------------------------
# CPM Modules
#------------------------------------------------------------------------------

# ++ MODULE: luna
CPM_AddModule("luna"
        GIT_REPOSITORY "https://github.com/DEGoodmanWilson/luna"
        GIT_TAG "v1.0.0-beta5")

CPM_Finish()

#deps
add_subdirectory(engine)

set(SQLITECPP_RUN_CPPLINT OFF)
set(SQLITECPP_RUN_CPPCHECK OFF)
add_subdirectory(SQLiteCpp)

set(SOURCE_FILES
        main.cpp
        app_web_controller.cpp
        app_web_controller.h
        app_persistent_store.cpp
        app_persistent_store.h
#        connection_thread_manager.cpp
#        connection_thread_manager.h
#        bot.cpp
#        bot.h
        )

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

target_link_libraries(redstone SQLiteCpp sqlite3 cpr engine {$CPM_LIBRARIES})
