cmake_minimum_required(VERSION 3.14)
project(SchedulerTests)

set(CMAKE_CXX_STANDARD 20)

# Add main code
add_library(scheduler STATIC main.cpp)

# GoogleTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Tests
add_executable(tests tests/test_scheduler.cpp)
target_link_libraries(tests scheduler ${GTEST_LIBRARIES} pthread)

enable_testing()
add_test(NAME SchedulerTests COMMAND tests)
