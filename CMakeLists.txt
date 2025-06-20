cmake_minimum_required(VERSION 3.20)

# CORRECTED: Added 'C' to the languages to support sqlite3.c
project(ZmqSqliteExampleProject VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
find_package(Threads REQUIRED) # Find pthreads, which is often required by SQLite on Linux

# --- Configure Subprojects BEFORE Making Them Available ---

# For libzmq: Force a SHARED library build
set(BUILD_SHARED ON CACHE BOOL "Force shared library build" FORCE)
set(BUILD_STATIC OFF CACHE BOOL "Force static library build" FORCE)
set(WITH_IPC OFF CACHE BOOL "Disable IPC transport on Windows" FORCE)

# For cppzmq: It's header-only, but disable its tests to prevent warnings.
set(CPPZMQ_BUILD_TESTS OFF CACHE BOOL "Disable cppzmq tests" FORCE)


# --- Let CMake Handle the Download and Build ---

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


# --- SQLite3 ---
# This is the new, robust method. We download the official amalgamation zip,
# which is guaranteed to contain the sqlite3.c and sqlite3.h files.
FetchContent_Declare(
        sqlite3
        URL          https://www.sqlite.org/2024/sqlite-amalgamation-3450300.zip
        # CORRECTED HASH - Updated to match the actual downloaded file from your error log.
        URL_HASH     SHA256=ea170e73e447703e8359308ca2e4366a3ae0c4304a8665896f068c736781c651
)
# This populates the source, fixing the deprecation warning.
FetchContent_MakeAvailable(sqlite3)

# The downloaded archive doesn't have a CMakeLists.txt, so FetchContent doesn't create
# a target. We create our own static library target from the downloaded source.
add_library(sqlite3_lib STATIC ${sqlite3_SOURCE_DIR}/sqlite3.c)
target_include_directories(sqlite3_lib PUBLIC ${sqlite3_SOURCE_DIR})


# --- Your Application ---

add_executable(deneme2 main.cpp)

# Link against our new sqlite3_lib target and other dependencies.
# Linking our library automatically provides the include paths to deneme2.
target_link_libraries(deneme2 PRIVATE cppzmq sqlite3_lib Threads::Threads)


# --- Copy DLL to EXE directory on Windows ---
# This remains the same. SQLite is now a static library, so it gets compiled
# directly into your executable and does not require a DLL to be copied.
if(WIN32)
    add_custom_command(
            TARGET deneme2 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:libzmq>
            $<TARGET_FILE_DIR:deneme2>
            COMMENT "Copying libzmq.dll to executable directory"
    )
endif()
