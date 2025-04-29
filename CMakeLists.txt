cmake_minimum_required(VERSION 3.14)
project(SchedulerTests)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)

find_package(GTest REQUIRED)
if(NOT TARGET GTest::GTest)
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

if(TARGET GTest::GTest)
    target_link_libraries(tests PRIVATE
        scheduler
        GTest::GTest
        GTest::Main
        Threads::Threads
    )
else()
    target_link_libraries(tests PRIVATE
        scheduler
        ${GTEST_BOTH_LIBRARIES}
        Threads::Threads
    )
    target_include_directories(tests PRIVATE ${GTEST_INCLUDE_DIRS})
endif()

include(GoogleTest)
gtest_discover_tests(tests)

enable_testing()
