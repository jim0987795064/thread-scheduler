#ifndef OS2021_API_H
#define OS2021_API_H

#define STACK_SIZE 8192

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "function_libary.h"


struct T_info
{
    char *job_name;
    char *p_function;
    char *B_Priority;
    char *priority;
    char *state;
    int cancel_mode;
    int TID;
    int time_quantum;
    int queueing_time;
    int event_waiting__time;
    int waiting_queue_target_time;
    int waiting_queue_enter_time;
    int event_id;
    bool testcancel;
    ucontext_t thread;

};

struct queue
{
    struct T_info *thread_info;
    struct queue *next;

};

void AddReadyQueue(struct T_info *new_thread);
void AddTerminateQueue(struct T_info *new_thread);
void ShowReadyQueue(int sig_num);
void scheduler(int sig_num);
int OS2021_ThreadCreate(char *job_name, char *p_function, char *priority, int cancel_mode);
void OS2021_ThreadCancel(char *job_name);
void OS2021_ThreadWaitEvent(int event_id);
void OS2021_ThreadSetEvent(int event_id);
void OS2021_ThreadWaitTime(int msec);
void OS2021_DeallocateThreadResource();
void OS2021_TestCancel();


void CreateContext(ucontext_t *, ucontext_t *, void *);
void SetTimer();
void Dispatcher();
void signal_process();
void StartSchedulingSimulation();

#endif
