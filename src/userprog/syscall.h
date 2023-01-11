#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "threads/thread.h"
#include <stdbool.h>
#include <stddef.h>

void syscall_init (void);

void check_address(void *addr);
void get_argument(void *esp, int *argument, int count);

void syscall_halt(void);
void syscall_exit(int status);
tid_t 
syscall_exec(const char *command);
bool syscall_create(const char* file_name, unsigned initial_size);
bool syscall_remove(const char* file_name);
int syscall_write(int fd, void *buffer, unsigned size);

#endif /* userprog/syscall.h */
