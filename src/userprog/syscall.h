#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "threads/thread.h"
#include <stdbool.h>
#include <stddef.h>

void syscall_init (void);

void syscall_halt(void);
void syscall_exit(int status);
int syscall_wait(tid_t tid);
bool syscall_create(const char* file_name, unsigned initial_size);
bool syscall_remove(const char* file_name);

static int read(void *src, void *dst, size_t bytes);

#endif /* userprog/syscall.h */
