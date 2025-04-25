#include <gtest/gtest.h>
#include "../scheduler.hpp"

TEST(SchedulerTest, SingleProcessCompletesCorrectly) {
    Scheduler scheduler;
    scheduler.addProcess({0, 0, 4});
    scheduler.run();

    auto proc = scheduler.getProcess(0);
    EXPECT_EQ(proc.endTime, 4);
    EXPECT_EQ(proc.waitingTime, 0);
}

TEST(SchedulerTest, TwoProcessesRoundRobin) {
    Scheduler scheduler;
    scheduler.addProcess({0, 0, 4});
    scheduler.addProcess({1, 0, 4});
    scheduler.run();

    auto p0 = scheduler.getProcess(0);
    auto p1 = scheduler.getProcess(1);

    EXPECT_EQ(p0.endTime, 8);
    EXPECT_EQ(p1.endTime, 8);
    EXPECT_EQ(p0.waitingTime + p1.waitingTime, 4);
}

TEST(SchedulerTest, TwoProcessesWithoutOverlap) {
    Scheduler scheduler;
    scheduler.addProcess({0, 0, 5});
    scheduler.addProcess({1, 0, 3});

    scheduler.run();
    
    auto p0 = scheduler.getProcess(0);
    auto p1 = scheduler.getProcess(1);

    EXPECT_EQ(p0.endTime, 8);
    EXPECT_EQ(p1.endTime, 7);
    
    EXPECT_EQ(p0.waitingTime, 3);
    EXPECT_EQ(p0.waitingTime, 4);
} 
