project(SchedulerTests)

set(CMAKE_CXX_STANDARD 20)

# Find Threads package (add this near your other find_package calls)
find_package(Threads REQUIRED)

# Add main code
add_library(scheduler STATIC
    main.cpp
    scheduler.cpp
)

# GoogleTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Tests
add_executable(tests tests/test_scheduler.cpp)

# Link scheduler library, GoogleTest AND Threads
target_link_libraries(tests
    scheduler
    GTest::GTest
    GTest::Main
    Threads::Threads
)

# Enable test discovery
include(GoogleTest)
gtest_discover_tests(tests)

enable_testing()
