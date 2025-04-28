cmake_minimum_required(VERSION 3.14)
project(SchedulerTests)

set(CMAKE_CXX_STANDARD 20)

# Add main code (including the relevant .cpp files)
add_library(scheduler STATIC
    main.cpp         # Keep main.cpp here as it contains the main logic
    scheduler.cpp    # Add scheduler.cpp
)

# GoogleTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Tests
add_executable(tests tests/test_scheduler.cpp)

# Link scheduler library and GoogleTest
target_link_libraries(tests scheduler ${GTEST_LIBRARIES} pthread)

enable_testing()
add_test(NAME SchedulerTests COMMAND tests)

