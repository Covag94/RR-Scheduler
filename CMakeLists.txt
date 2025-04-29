cmake_minimum_required(VERSION 3.14)  # Required for proper GTest integration
project(SchedulerTests)

set(CMAKE_CXX_STANDARD 20)

# Find Threads package
find_package(Threads REQUIRED)

# Find GTest - try both module and config modes
find_package(GTest REQUIRED)
if(NOT TARGET GTest::GTest)
    # Fallback for older GTest installations
    include(GoogleTest)
    find_package(GTest REQUIRED)
endif()

# Main library
add_library(scheduler STATIC
    main.cpp
    scheduler.cpp
)

# Test executable
add_executable(tests tests/test_scheduler.cpp)

# Modern target linking with fallback
if(TARGET GTest::GTest)
    target_link_libraries(tests PRIVATE
        scheduler
        GTest::GTest
        GTest::Main
        Threads::Threads
    )
else()
    # Fallback for older versions
    target_link_libraries(tests PRIVATE
        scheduler
        ${GTEST_BOTH_LIBRARIES}
        Threads::Threads
    )
    target_include_directories(tests PRIVATE ${GTEST_INCLUDE_DIRS})
endif()

# Enable test discovery
include(GoogleTest)
gtest_discover_tests(tests)

enable_testing()
