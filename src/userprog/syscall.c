#include "userprog/syscall.h"
#include <stdbool.h>
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

/*System call initializer*/
void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/*Handler for system commands.*/
static void
syscall_handler (struct intr_frame *f UNUSED)
{
  /* References: J,Choi(2014), pintos. Available from: https://github.com/wookayin/pintos [accessed on 27/11/22]*/
  int syscall_number;
  ASSERT( sizeof(syscall_number) == 4 ); /*assuming x86*/

  /*The system call number is in the 32-bit word at the caller's stack pointer.*/
  read(f->esp, &syscall_number, sizeof(syscall_number));

  /*Store the esp, which is needed in the page fault handler.*/
  thread_current()->current_esp = f->esp;

  switch (syscall_number) {
  case SYS_HALT: // 0
    {
      /*Terminate PintOS*/
      syscall_halt();
      break;
    }
    case SYS_WAIT: // 2
    {
      tid_t tid;
      /*Get argument*/
      read(f->esp + 4, &tid, sizeof(tid_t));
      /*Wait for child process*/
      f->eax = syscall_wait(tid);
      break;
    }

    case SYS_CREATE: // 3
    {
      const char* file_name;
      unsigned initial_size;

      /*Get file name and size*/
      read(f->esp + 4, &file_name, sizeof(file_name));
      read(f->esp + 8, &initial_size, sizeof(initial_size));

      /*Create file*/
      f->eax =  syscall_create(file_name, initial_size);
      break;
    }

    case SYS_REMOVE: // 4
    {
      const char* file_name;
      bool return_code;

      /*Get file name*/
      read(f->esp + 4, &file_name, sizeof(file_name));

      /*Remove file*/
      f->eax = syscall_remove(file_name);
      break;
    }
    /* Unimplemented system calls */
    default:
        printf("ERROR: system call ( %d ) has not implemented!\n", syscall_number);

    /* Terminate. */
    syscall_exit(-1);
    break;
  }
}


/********SYSTEMCALLS********/

/* Halt */
void syscall_halt(void) {
  shutdown_power_off(); /* From shutdown.h */
}

/* Exit */
void syscall_exit(int status) {
  struct thread *current_process=thread_current();
  current_process->process_exit_status = status;

  printf("%s: exit(%d)\n",current_process->name,status);

  thread_exit();
}

/* Wait */
int syscall_wait(tid_t tid)
{
  return process_wait(tid);
}

/* Create File */
bool syscall_create(const char* file_name, unsigned initial_size) {
  bool if_created = false;
  if( filesys_create(file_name, initial_size)==true){
    if_created = true;
  }
  return if_created;
}

/* Remove File */
bool syscall_remove(const char* file_name) {
  bool if_removed = false;
  if( filesys_remove(file_name)==true){
    if_removed = true;
  }
  return if_removed;
}


/****OTHER FUNCTIONS****/
static int read(void *src, void *dst, size_t bytes)
{
  int32_t value;
  size_t i;
  for (i = 0; i < bytes; i++)
  {
    value = get_user(src + i);
    if (value == -1) // segfault or invalid memory access
      fail_invalid_access();

    *(char *)(dst + i) = value & 0xff;
  }
  return (int)bytes;
}
/* Reads a byte at user virtual address UADDR.
   UADDR must be below PHYS_BASE.
   Returns the byte value if successful, -1 if a segfault
   occurred. */
static int
get_user (const uint8_t *uaddr)
{
  if(!is_user_vaddr(uaddr))
    return -1;
  int result;
  asm ("movl $1f, %0; movzbl %1, %0; 1:"
       : "=&a" (result) : "m" (*uaddr));
  return result;
}
