cmake_minimum_required(VERSION 3.20)

project(ZmqSqliteExampleProject VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
find_package(Threads REQUIRED)

set(BUILD_SHARED ON CACHE BOOL "Force shared library build" FORCE)
set(BUILD_STATIC OFF CACHE BOOL "Force static library build" FORCE)
set(WITH_IPC OFF CACHE BOOL "Disable IPC transport on Windows" FORCE)

set(CPPZMQ_BUILD_TESTS OFF CACHE BOOL "Disable cppzmq tests" FORCE)

FetchContent_Declare(
        libzmq
        GIT_REPOSITORY https://github.com/zeromq/libzmq.git
        GIT_TAG        v4.3.5
)
FetchContent_MakeAvailable(libzmq)

FetchContent_Declare(
        cppzmq
        GIT_REPOSITORY https://github.com/zeromq/cppzmq.git
        GIT_TAG        v4.10.0
)
FetchContent_MakeAvailable(cppzmq)

FetchContent_Declare(
        sqlite3
        URL          https://www.sqlite.org/2024/sqlite-amalgamation-3450300.zip
        URL_HASH     SHA256=ea170e73e447703e8359308ca2e4366a3ae0c4304a8665896f068c736781c651
)

FetchContent_MakeAvailable(sqlite3)


add_library(sqlite3_lib STATIC ${sqlite3_SOURCE_DIR}/sqlite3.c)
target_include_directories(sqlite3_lib PUBLIC ${sqlite3_SOURCE_DIR})

add_executable(deneme2 main.cpp)

target_link_libraries(deneme2 PRIVATE cppzmq sqlite3_lib Threads::Threads)



if(WIN32)
    add_custom_command(
            TARGET deneme2 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:libzmq>
            $<TARGET_FILE_DIR:deneme2>
            COMMENT "Copying libzmq.dll to executable directory"
    )
endif()
