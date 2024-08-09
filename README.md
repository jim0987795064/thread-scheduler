# Explanation of the Code

The provided code is an implementation of a basic operating system thread scheduler using context switching. Below is a detailed explanation of its components and functionality.

## Key Components

### 1. Thread Information Structure (`T_info`)
- This structure holds the information for each thread, including attributes like job name, function pointer, priority, state, thread ID, time quantum, and event-related data.
- Each thread's context is stored in a `ucontext_t` structure which is used for context switching.

### 2. Queues
- Multiple queues are used to manage the threads based on their priority or state:
  - **`high_ready_queue`**, **`medium_ready_queue`**, and **`low_ready_queue`**: Store threads that are ready to run, categorized by priority.
  - **`event_high_queue`**, **`event_medium_queue`**, and **`event_low_queue`**: Store threads waiting for an event, categorized by priority.
  - **`running_queue`**: Holds the currently running thread.
  - **`terminate_queue`**: Stores threads that have been terminated and are pending resource deallocation.
  - **`waiting_queue`**: Stores threads that are waiting for a specific amount of time.

### 3. Thread Creation (`OS2021_ThreadCreate`)
- This function creates a new thread by allocating memory for the `T_info` structure and setting its attributes.
- The function context is created using `CreateContext`.
- The new thread is added to the appropriate ready queue based on its priority.

### 4. Context Switching
- The context of each thread is managed using `ucontext_t`. The function `swapcontext` is used to switch between different thread contexts, allowing the simulation of multitasking.

### 5. Thread Management Functions
- **`AddReadyQueue`**: Adds a thread to the appropriate ready queue based on its priority.
- **`AddTerminateQueue`**: Adds a thread to the terminate queue, marking it for resource deallocation.
- **`OS2021_ThreadCancel`**: Cancels a thread either immediately or on the next cancellation point depending on the thread's cancellation mode.
- **`OS2021_ThreadWaitEvent`** and **`OS2021_ThreadSetEvent`**: Manage threads that are waiting for or signaling an event, moving them between the ready and event queues.
- **`OS2021_ThreadWaitTime`**: Puts a thread into the waiting queue for a specified amount of time.
- **`OS2021_DeallocateThreadResource`**: Deallocates memory for threads that have been terminated.

### 6. Scheduler and Timer Management
- **`scheduler`**: Continuously checks the ready queues and selects the next thread to run.
- **`SetTimer`**: Configures a timer that generates signals at regular intervals, used to track the passage of time for scheduling purposes.
- **`timer`**: Increments time-related variables and checks if threads should be moved between queues based on time or event conditions.

### 7. Signal Handling
- Custom signal handlers are set up to display queue contents or thread states when certain signals (e.g., `SIGQUIT` or `SIGTSTP`) are received.
- These signal handlers allow the user to inspect the state of the scheduler and its threads.

### 8. Initialization and Simulation
- **`ParseJson`**: Reads the thread information from a JSON file and creates the initial threads.
- **`StartSchedulingSimulation`**: Initializes the necessary queues and structures, sets up signal handling, and starts the scheduling loop by setting the initial context.

## Summary

This code provides a basic simulation of thread management in an operating system, including context switching, thread prioritization, event handling, and resource management. It demonstrates how threads can be scheduled and managed in a simple round-robin or priority-based fashion.
