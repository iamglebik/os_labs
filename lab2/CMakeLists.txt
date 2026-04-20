cmake_minimum_required(VERSION 3.10)
project(CMakelab2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(threads_lib
    threads.cpp
    Array.cpp
)
target_include_directories(threads_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Main main.cpp)
target_link_libraries(Main PRIVATE threads_lib)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(UnitTests tests.cpp)
target_link_libraries(UnitTests PRIVATE threads_lib gtest_main)
include(GoogleTest)
gtest_discover_tests(UnitTests)
