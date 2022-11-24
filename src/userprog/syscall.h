#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);

/*PROTOTYPES*/
void syscall_halt(void);
void syscall_exit(int status);
int syscall_wait(pid_t pid);
bool syscall_create(const char* file_name, unsigned initial_size);
bool syscall_remove(const char* file_name, unsigned initial_size);

#endif /* userprog/syscall.h */
