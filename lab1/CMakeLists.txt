cmake_minimum_required(VERSION 3.14)
project(EmployeeRecordsSystem VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Жестко убираем Wextra для MSVC
if(MSVC)
    # Сбрасываем все флаги
    set(CMAKE_CXX_FLAGS "/W3 /EHsc /MD")
    set(CMAKE_C_FLAGS "/W3 /MD")
    
    # Для Google Test
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    
    # Отключаем предупреждения для Google Test
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4267 /wd4244 /wd4305")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Executables
add_executable(Creator 
    src/Creator.cpp
    include/employee.h
    include/FileHandler.h
    include/InputValidator.h
)

add_executable(Reporter 
    src/Reporter.cpp
    include/employee.h
    include/FileHandler.h
    include/ReportGenerator.h
    include/InputValidator.h
)

add_executable(Main 
    src/Main.cpp
    include/employee.h
    include/FileHandler.h
    include/InputValidator.h
)

# Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(googletest)

# Test executable
add_executable(test_runner 
    tests/test_runner.cpp
    tests/test_creator.cpp
    tests/test_reporter.cpp
    tests/test_integration.cpp
    include/employee.h
    include/FileHandler.h
    include/ReportGenerator.h
)

target_link_libraries(test_runner 
    GTest::gtest_main
    GTest::gmock
)

target_include_directories(test_runner PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

enable_testing()
add_test(NAME all_tests COMMAND test_runner)