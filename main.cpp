#include "scheduler.hpp"
#include <iostream>

int main() {
  // Create Scheduler instance
  Scheduler *scheduler = new Scheduler(new RoundRobinStrategy(4));

  // Add simulated processes
  scheduler->addProcess({0, 0, 5}); // pid, start, burst
  scheduler->addProcess({1, 0, 3});
  scheduler->addProcess({2, 4, 7});

  //  scheduler->printQueue();

  std::cout << " " << std::endl;
  std::cout << "Running scheduling" << std::endl;
  std::cout << " " << std::endl;

  // Run the scheduling loop
  scheduler->run();

  // Print final stats
  scheduler->printProcessesMetaData();
  return 0;
}
