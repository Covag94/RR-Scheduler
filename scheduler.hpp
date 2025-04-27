#include <deque>
#include <iomanip>
#include <unordered_map>
#include <vector>

struct Process {
  unsigned int pid;
  unsigned int startTime;
  unsigned int burstTime;
  int waitingTime = 0;
  int endTime = -1;

  Process(const unsigned int pid, const unsigned int startTime,
          const unsigned int burstTime) {
    this->pid = pid;
    this->startTime = startTime;
    this->burstTime = burstTime;
  }
};

class Scheduler {
private:
  unsigned int currentTime;
  const unsigned int timeQuantum;
  std::deque<unsigned int> readyQueue; // PIDs
  std::vector<Process> allProcesses;
  std::unordered_map<unsigned int, int> pidToVecIndex;

  void updateWaitingTimes(const Process &currentProcess,
                          const size_t runningTime) {
    for (auto &proc : allProcesses) {
      if (currentProcess.pid == proc.pid) {
        proc.burstTime = currentProcess.burstTime;
        continue;
      }

      if (proc.burstTime > 0) {
        proc.waitingTime += runningTime;
      }
    }
  }

  size_t processTimeSlice(Process &currentProcess) {
    size_t runningTime = 0;

    for (size_t dt = 1; dt <= timeQuantum; ++dt) {
      currentProcess.burstTime--;
      runningTime++;
      currentTime++;

      // Current process is finished - exit early
      if (currentProcess.burstTime == 0) {
        break;
      }
    }

    return runningTime;
  }

  void markProcComplete(const unsigned int idx,
                        const unsigned int currentTime) {
    allProcesses[idx].endTime = currentTime;
  }

public:
  Scheduler() : currentTime(0), timeQuantum(4) {}

  void addProcess(Process proc) {
    if (proc.burstTime > 0) {
      const auto pid = proc.pid;

      allProcesses.push_back(std::move(proc));
      pidToVecIndex[pid] = allProcesses.size() - 1;
    }
  }

  // Main logic of RR scheduling
  void run() {
    // First sort allProcesses according to startTime
    std::sort(allProcesses.begin(), allProcesses.end(),
              [](const Process &a, const Process &b) {
                return a.startTime < b.startTime;
              });

    size_t nextToPush = 0;

    while (!readyQueue.empty() && nextToPush < allProcesses.size()) {
      while (nextToPush < allProcesses.size() &&
             allProcesses[nextToPush].startTime <= currentTime) {
        const auto proc = allProcesses[nextToPush];
        readyQueue.push_back(proc.pid);
        nextToPush++;
      }

      if (readyQueue.empty()) {
        currentTime++;
        continue;
      }

      const auto pid = readyQueue.front();
      const auto vecIdx = pidToVecIndex[pid];
      auto &proc = allProcesses[vecIdx];
      readyQueue.pop_front();

      const size_t runningTime = processTimeSlice(proc);

      // Needs further processing
      if (proc.burstTime > 0) {
        readyQueue.push_back(pid);
      } else {
        markProcComplete(vecIdx, currentTime);
      }

      // Update waiting times and finish time
      updateWaitingTimes(proc, runningTime);
    }
  }

  const Process &getProcess(unsigned int pid) const {
    return allProcesses.at(pidToVecIndex.at(pid));
  }

  void printProcess(const unsigned int pid) {
    const auto i_proc = pidToVecIndex[pid];
    const auto &proc = allProcesses[i_proc];

    std::cout << std::setw(12) << proc.pid << std::setw(12) << proc.startTime
              << std::setw(12) << proc.endTime << std::setw(12)
              << proc.burstTime << std::setw(12) << proc.waitingTime << "\n";
  }

  void printQueue() {
    std::cout
        << "      ==========================Process========================"
        << std::endl;
    std::cout << std::setw(12) << "PID" << std::setw(12) << "Start"
              << std::setw(12) << "End" << std::setw(12) << "Burst"
              << std::setw(12) << "Waiting" << "\n";

    for (const auto pid : readyQueue) {
      printProcess(pid);
    }
  }

  void printProcessesMetaData() {
    std::cout
        << "      ==========================Process========================"
        << std::endl;
    std::cout << std::setw(12) << "PID" << std::setw(12) << "Start"
              << std::setw(12) << "End" << std::setw(12) << "Burst"
              << std::setw(12) << "Waiting" << "\n";
    for (const auto &proc : allProcesses) {
      const auto pid = proc.pid;

      printProcess(pid);
    }
  }

  void reset() {
    allProcesses.clear();
    readyQueue.clear();
    pidToVecIndex.clear();
    currentTime = 0;
  }
};
