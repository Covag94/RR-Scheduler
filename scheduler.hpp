#pragma once

#include <deque>
#include <unordered_map>
#include <vector>

class SchedulerStrategy;

struct Process {
  unsigned int pid;
  unsigned int startTime;
  unsigned int burstTime;
  int waitingTime = 0;
  int endTime = -1;

  Process(unsigned int pid, unsigned int startTime, unsigned int burstTime);
};

class Scheduler {
private:
  unsigned int currentTime;
  std::deque<unsigned int> readyQueue; // PIDs
  std::vector<Process> allProcesses;
  std::unordered_map<unsigned int, int> pidToVecIndex;
  SchedulerStrategy *strategy;

public:
  void setCurrentTime(const unsigned int newTime);
  void updateWaitingTimes(const Process &currentProcess,
                          const size_t runningTime);

  Process &getProcess(const unsigned int pid);
  size_t processTimeSlice(Process &currentProcess,
                          const unsigned int timeQuantum, const std::vector<size_t>& sortedIndices, size_t& nextToPush);
  void markProcComplete(const unsigned int idx, const unsigned int currentTime);

public:
  Scheduler(SchedulerStrategy *strat);
  ~Scheduler();

  std::deque<unsigned int> &getQueue();
  std::vector<Process> &getProcesses();
  std::unordered_map<unsigned int, int> &getPIDToVecIndex();
  unsigned int getCurrentTime();

  void addProcess(Process proc);
  void run();
  const Process &getProcess(unsigned int pid) const;
  void printProcess(const unsigned int pid);
  void printQueue();
  void printProcessesMetaData();
  void reset();
};

class SchedulerStrategy {
public:
  virtual void run(Scheduler &scheduler) = 0;
  virtual ~SchedulerStrategy() = default;
};

class RoundRobinStrategy : public SchedulerStrategy {
private:
  unsigned int timeQuantum;

public:
  RoundRobinStrategy(unsigned int quantum);
  void run(Scheduler &scheduler) override;
};

enum class SchedulerType { RoundRobin };
Scheduler *SchedulerFactory(SchedulerType type);
