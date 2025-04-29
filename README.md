# Round-Robin Process Scheduler

This project implements a simple **Round-Robin CPU Scheduler** in C++. It simulates how an operating system schedules multiple processes using time slices and keeps track of various process-related metadata such as start time, end time, burst time, and waiting time.

##  Overview

This scheduler is built around the Round-Robin algorithm, where each process is given a fixed time quantum to execute. If a process does not finish within its time slice, it is placed back into the queue for another turn. The scheduler accounts for processes that arrive at different times and updates waiting times accordingly.

## Key Components

### `Process`
A struct representing a process with the following fields:
- `pid`: Process ID
- `startTime`: When the process arrives
- `burstTime`: Total CPU time required
- `waitingTime`: Time spent waiting in the queue
- `endTime`: Time at which the process completes

### `Scheduler`
The core scheduler that manages:
- A `readyQueue` of processes by PID
- A list of all `Process` objects
- A strategy (e.g., Round Robin) to execute scheduling logic
- Functions to manage current time, queue updates, and execution

### `SchedulerStrategy`
An abstract base class for pluggable scheduling strategies.

### `RoundRobinStrategy`
Implements the Round-Robin scheduling logic using a fixed time quantum. During each time slice, it:
- Executes a process for up to the time quantum
- Adds newly arrived processes to the queue dynamically
- Re-queues unfinished processes

### `SchedulerFactory`
A simple factory that currently supports instantiating a `Scheduler` using the `RoundRobinStrategy`.

## Tests

Unit tests are included to validate scheduling behavior across various arrival and burst time configurations. 

## Build Instructions

Use `CMake` to build:

```bash
mkdir build && cd build
cmake ..
make
./tests

