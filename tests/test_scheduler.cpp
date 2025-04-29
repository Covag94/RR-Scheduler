#include "../scheduler.hpp"
#include <gtest/gtest.h>

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  std::cout << "Running "
            << ::testing::UnitTest::GetInstance()->total_test_case_count()
            << " test cases.\n";
  return RUN_ALL_TESTS();
}

TEST(SchedulerTest, SingleProcessCompletesCorrectly) {
  Scheduler *scheduler = new Scheduler(new RoundRobinStrategy(4));
  scheduler->addProcess({0, 0, 4});
  scheduler->run();

  auto proc = scheduler->getProcess(0);
  EXPECT_EQ(proc.endTime, 4);
  EXPECT_EQ(proc.waitingTime, 0);
}

TEST(SchedulerTest, TwoProcessesRoundRobin) {
  Scheduler *scheduler = new Scheduler(new RoundRobinStrategy(4));
  scheduler->addProcess({0, 0, 4});
  scheduler->addProcess({1, 0, 4});
  scheduler->run();

  auto p0 = scheduler->getProcess(0);
  auto p1 = scheduler->getProcess(1);

  EXPECT_EQ(p0.endTime, 4);
  EXPECT_EQ(p1.endTime, 8);
  EXPECT_EQ(p0.waitingTime, 0);
  EXPECT_EQ(p1.waitingTime, 4);
}

TEST(SchedulerTest, TwoProcessesWithoutOverlap) {
  Scheduler *scheduler = new Scheduler(new RoundRobinStrategy(4));
  scheduler->addProcess({0, 0, 5});
  scheduler->addProcess({1, 0, 3});

  scheduler->run();

  auto p0 = scheduler->getProcess(0);
  auto p1 = scheduler->getProcess(1);

  EXPECT_EQ(p0.endTime, 8);
  EXPECT_EQ(p1.endTime, 7);

  EXPECT_EQ(p0.waitingTime, 3);
  EXPECT_EQ(p1.waitingTime, 4);
}

TEST(SchedulerTest, ZeroBurstProcessIgnored) {
  Scheduler *scheduler = new Scheduler(new RoundRobinStrategy(4));
  scheduler->addProcess({0, 0, 0});
  scheduler->run();
  try {
    auto p = scheduler->getProcess(0);
    std::cout << "Unexpectedly retrieved process with endTime: " << p.endTime
              << std::endl;
  } catch (const std::out_of_range &e) {
    std::cout << "Caught expected exception: " << e.what() << std::endl;
  }
  EXPECT_THROW(scheduler->getProcess(0), std::out_of_range);
}

TEST(SchedulerTest, ProcessStartsLaterJumpsTime) {
  Scheduler scheduler(new RoundRobinStrategy(4));
  scheduler.addProcess({0, 5, 2});
  scheduler.addProcess({1, 10, 2});
  scheduler.run();

  EXPECT_EQ(scheduler.getCurrentTime(), 12); // 5-6: P0, 10-12: P1
}

TEST(SchedulerTest, ExactQuantumCompletion) {
  Scheduler scheduler(new RoundRobinStrategy(4));
  scheduler.addProcess({0, 0, 4});
  scheduler.run();

  auto p0 = scheduler.getProcess(0);
  EXPECT_EQ(p0.endTime, 4);
  EXPECT_EQ(p0.waitingTime, 0);
}

TEST(SchedulerTest, ProcessRequiresMultipleQuanta) {
  Scheduler scheduler(new RoundRobinStrategy(4));
  scheduler.addProcess({0, 0, 7}); // Needs 2 quanta (4 + 3)
  scheduler.run();

  auto p0 = scheduler.getProcess(0);
  EXPECT_EQ(p0.endTime, 7);
  EXPECT_EQ(p0.waitingTime, 0); // No other processes
}

TEST(SchedulerTest, ThreeProcessesRoundRobinOrder) {
  Scheduler *scheduler =
      new Scheduler(new RoundRobinStrategy(2)); // Smaller quantum
  scheduler->addProcess({0, 0, 3});
  scheduler->addProcess({1, 0, 3});
  scheduler->addProcess({2, 0, 3});
  scheduler->run();

  // Expected execution order: 0,1,2,0,1,2,0,1,2
  EXPECT_EQ(scheduler->getProcess(0).endTime, 7);
  EXPECT_EQ(scheduler->getProcess(1).endTime, 8);
  EXPECT_EQ(scheduler->getProcess(2).endTime, 9);

  EXPECT_EQ(scheduler->getProcess(0).waitingTime, 4);
  EXPECT_EQ(scheduler->getProcess(1).waitingTime, 5);
  EXPECT_EQ(scheduler->getProcess(2).waitingTime, 6);
}

TEST(SchedulerTest, WaitingTimeAccumulation) {
  Scheduler scheduler(new RoundRobinStrategy(2));
  scheduler.addProcess({0, 0, 4});
  scheduler.addProcess({1, 0, 2});
  scheduler.run();

  // P0 waits while P1 runs (2 units)
  EXPECT_EQ(scheduler.getProcess(0).waitingTime, 2);
  // P1 waits while P0 runs (first 2 units)
  EXPECT_EQ(scheduler.getProcess(1).waitingTime, 2);
}

TEST(SchedulerTest, ResetClearsState) {
  Scheduler scheduler(new RoundRobinStrategy(4));
  scheduler.addProcess({0, 0, 2});
  scheduler.run();
  scheduler.reset();

  EXPECT_EQ(scheduler.getCurrentTime(), 0);
  EXPECT_TRUE(scheduler.getProcesses().empty());
  EXPECT_TRUE(scheduler.getQueue().empty());
}

TEST(SchedulerTest, LateArrivingProcess) {
  Scheduler scheduler(new RoundRobinStrategy(4));
  scheduler.addProcess({0, 0, 2});
  scheduler.addProcess({1, 3, 2}); // Arrives at t=3
  scheduler.run();

  EXPECT_EQ(scheduler.getProcess(0).endTime, 2);
  EXPECT_EQ(scheduler.getProcess(1).endTime, 5);
}

TEST(SchedulerTest, MixedArrivalTimes) {
  Scheduler scheduler(new RoundRobinStrategy(3));
  scheduler.addProcess({0, 0, 5});
  scheduler.addProcess({1, 2, 4}); // Arrives at t=2
  scheduler.addProcess({2, 5, 2}); // Arrives at t=5
  scheduler.run();

  EXPECT_EQ(scheduler.getProcess(0).endTime, 8);
  EXPECT_EQ(scheduler.getProcess(1).endTime, 11);
  EXPECT_EQ(scheduler.getProcess(2).endTime, 10);
}
