#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

typedef int pid_t;

#define PID_ERROR	((pid_t) -1)
#define PID_INITIALIZING ((pid_t) -2)

pid_t process_execute (const char *command);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

/* PCB : PROCESS CONTROL BLOCK */
struct process_control_block {
  //Unique Process ID
  pid_t pid;

  //Command Line
  const char* cmdline;     

  //Thread child_list element
  struct list_elem elem;

  //Parent thread
  struct thread* parent_thread;

  //Shows parent process is waiting on
  bool waiting;             
  //Shows process is done
  bool exited;              
  //Shows parent process has terminated
  bool orphan;              
  //The exit code passed from exit(), when exited = true 
  int32_t exitcode;        

  /* Synchronization */
  //Semaphore used between start_process() and process_execute() 
  struct semaphore sema_initialization;   
  //Semaphore used for wait() : parent blocks until child exits */
  struct semaphore sema_wait;           

};

#endif /* userprog/process.h */
