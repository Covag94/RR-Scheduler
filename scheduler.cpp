#include "scheduler.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>

// Process
Process::Process(unsigned int pid, unsigned int startTime,
                 unsigned int burstTime)
    : pid(pid), startTime(startTime), burstTime(burstTime) {}

// Scheduler
Scheduler::Scheduler(SchedulerStrategy *strat)
    : currentTime(0), strategy(strat) {}

Scheduler::~Scheduler() { delete strategy; }

void Scheduler::setCurrentTime(const unsigned int newTime) {
  currentTime = newTime;
}

void Scheduler::updateWaitingTimes(const Process &currentProcess,
                                   const size_t runningTime) {
  for (auto pid : readyQueue) {
    auto &proc = allProcesses[pidToVecIndex[pid]];

    // Do NOT update waiting time for part of process added at back of queue
    if (currentProcess.pid != proc.pid) {
      proc.waitingTime += runningTime;
    }
  }
}

Process &Scheduler::getProcess(const unsigned int pid) {
  auto it = pidToVecIndex.find(pid);
  if (it != pidToVecIndex.end()) {
    size_t idx = it->second;
    if (idx < allProcesses.size()) {
      if (allProcesses[idx].pid == pid) {
        return allProcesses[idx];
      } else {
        throw std::out_of_range("PID mapping mismatch.");
      }
    } else {
      throw std::out_of_range("Index in pidToVecIndex out of bounds.");
    }
  } else {
    throw std::out_of_range("PID not found in pidToVecIndex.");
  }
}

size_t Scheduler::processTimeSlice(Process &currentProcess,
                                   const unsigned int timeQuantum, const std::vector<size_t> &sortedIndices,
                                   size_t& nextToPush) {
  size_t runningTime = 0;

  for (size_t dt = 1; dt <= timeQuantum && currentProcess.burstTime > 0; ++dt) {
    currentProcess.burstTime--;
    runningTime++;
    currentTime++;

    // Need to check whether process arrives during current process in on CPU
    while (nextToPush < sortedIndices.size() &&
           allProcesses[sortedIndices[nextToPush]].startTime <= currentTime) {
      readyQueue.push_back(allProcesses[sortedIndices[nextToPush]].pid);
      nextToPush++;
    }
  }

  return runningTime;
}

void Scheduler::markProcComplete(const unsigned int idx,
                                 const unsigned int currentTime) {
  allProcesses[idx].endTime = currentTime;
}

std::deque<unsigned int> &Scheduler::getQueue() { return readyQueue; }

std::vector<Process> &Scheduler::getProcesses() { return allProcesses; }

std::unordered_map<unsigned int, int> &Scheduler::getPIDToVecIndex() {
  return pidToVecIndex;
}

unsigned int Scheduler::getCurrentTime() { return currentTime; }

void Scheduler::addProcess(Process proc) {
  if (proc.burstTime > 0) {
    const auto pid = proc.pid;
    allProcesses.push_back(std::move(proc));
    pidToVecIndex[pid] = allProcesses.size() - 1;
    std::cout << "Added process with pid " << pid << std::endl;
  } else {
    std::cout << "Ignored process with burstTime 0 (pid = " << proc.pid
              << ")\n";
  }
}

void Scheduler::run() { strategy->run(*this); }

void Scheduler::printProcess(const unsigned int pid) {
  const auto i_proc = pidToVecIndex[pid];
  const auto &proc = allProcesses[i_proc];
  std::cout << std::setw(12) << proc.pid << std::setw(12) << proc.startTime
            << std::setw(12) << proc.endTime << std::setw(12) << proc.burstTime
            << std::setw(12) << proc.waitingTime << "\n";
}

void Scheduler::printQueue() {
  std::cout
      << "      ==========================Process========================\n";
  std::cout << std::setw(12) << "PID" << std::setw(12) << "Start"
            << std::setw(12) << "End" << std::setw(12) << "Burst"
            << std::setw(12) << "Waiting\n";
  for (const auto pid : readyQueue) {
    printProcess(pid);
  }
}

void Scheduler::printProcessesMetaData() {
  std::cout
      << "      ==========================Process========================\n";
  std::cout << std::setw(12) << "PID" << std::setw(12) << "Start"
            << std::setw(12) << "End" << std::setw(12) << "Burst"
            << std::setw(12) << "Waiting\n";
  for (const auto &proc : allProcesses) {
    printProcess(proc.pid);
  }
}

void Scheduler::reset() {
  allProcesses.clear();
  readyQueue.clear();
  pidToVecIndex.clear();
  currentTime = 0;
}

// RoundRobinStrategy
RoundRobinStrategy::RoundRobinStrategy(unsigned int quantum)
    : timeQuantum(quantum) {}

void RoundRobinStrategy::run(Scheduler &scheduler) {
  auto &allProcesses = scheduler.getProcesses();

  // Need to sort all processes according to startTime
  std::vector<size_t> sortedIndices(allProcesses.size());
  std::iota(sortedIndices.begin(), sortedIndices.end(), 0);
  std::sort(sortedIndices.begin(), sortedIndices.end(),
            [&allProcesses](size_t a, size_t b) {
              return allProcesses[a].startTime < allProcesses[b].startTime;
            });

  size_t nextToPush = 0;
  auto &readyQueue = scheduler.getQueue();

  while (!readyQueue.empty() || nextToPush < sortedIndices.size()) {
    while (nextToPush < sortedIndices.size() &&
           allProcesses[sortedIndices[nextToPush]].startTime <=
               scheduler.getCurrentTime()) {
      readyQueue.push_back(allProcesses[sortedIndices[nextToPush]].pid);
      nextToPush++;
    }

    // If queue empty, need to fast-forward to next available process or stop if
    // no processes are available
    if (readyQueue.empty()) {
      if (nextToPush < sortedIndices.size()) {
        scheduler.setCurrentTime(
            allProcesses[sortedIndices[nextToPush]].startTime);
        continue;
      }
      break;
    }

    const auto pid = readyQueue.front();
    readyQueue.pop_front();
    auto &proc = scheduler.getProcess(pid);


    // Give CPU time to first process in queue
    const size_t runningTime = scheduler.processTimeSlice(proc, timeQuantum, sortedIndices, nextToPush);

    // Time quantum not enough - need to re-queue remaining part of process
    if (proc.burstTime > 0) {
      readyQueue.push_back(pid);
    } else {
      scheduler.markProcComplete(scheduler.getPIDToVecIndex()[pid],
                                 scheduler.getCurrentTime());
    }

    // Update meta-data
    scheduler.updateWaitingTimes(proc, runningTime);
  }
}

// Factory implementation
Scheduler *SchedulerFactory(SchedulerType type) {
  switch (type) {
  case SchedulerType::RoundRobin:
    return new Scheduler(new RoundRobinStrategy(4));
  default:
    return nullptr;
  }
}
