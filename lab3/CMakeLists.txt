cmake_minimum_required(VERSION 3.14)
project(Os_lab3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(Main
    src/shared_array.cpp
    src/marker_manager.cpp
)

if (WIN32)
    target_link_libraries(Main PRIVATE ws2_32)
endif()

add_executable(ThreadSyncMain src/main.cpp)
target_link_libraries(ThreadSyncMain PRIVATE Main)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(Tests
    tests/tests.cpp
)
target_include_directories(Tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Tests PRIVATE Main gtest_main)

include(GoogleTest)
gtest_discover_tests(Tests)