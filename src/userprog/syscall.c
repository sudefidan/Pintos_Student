#include "userprog/syscall.h"
#include <stdbool.h>
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include <devices/shutdown.h>
#include <filesys/filesys.h>
#include <filesys/file.h>
#include <userprog/process.h>
#include <devices/input.h>

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
  int syscall_number;
  int argument[5];
  void *esp = f->esp;

  check_address(esp);
  syscall_number = *(int *)esp;

  switch (syscall_number) {
	case SYS_HALT: // 0
        /*Terminate PintOS*/
        syscall_halt();
        break;
    case SYS_EXIT: //1
		get_argument(esp,argument,1);
		syscall_exit(argument[0]);
		break;
    case SYS_EXEC: // 2 (TODO: Change to execute)
        get_argument(esp,argument,1);
		check_address((void *)argument[0]);
		f->eax = syscall_exec((const char *)argument[0]);
		break;
    case SYS_CREATE: // 4
        get_argument(esp,argument,2);
		check_address((void *)argument[0]);
		f->eax = syscall_create((const char *)argument[0],(unsigned)argument[1]);
        break;
    case SYS_REMOVE: // 5
        get_argument(esp,argument,1);
		check_address((void *)argument[0]);
		f->eax=syscall_remove((const char *)argument[0]);
		break;
    case SYS_WRITE: //9
		get_argument(esp,argument,3);
		check_address((void *)argument[1]);
		f->eax = syscall_write(argument[0],(void *)argument[1],(unsigned)argument[2]);
		break;
    /* Unimplemented system calls 	*/
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

/* Execute */
/* create child process and wait until childprocess is loaded */
/* runs the executable whose name is given */
tid_t
syscall_exec(const char *argument)
{
	printf("System executing %s ...\n", argument);

	/*dynamically create struct child*/
	struct thread *child_process;
	/*new process'c program id*/
	tid_t pid;
	
	pid = process_execute(argument);
	/* the child calls sema_up after the done in load()*/
	child_process = get_child_process(pid);
	/*Call sema_down before we return, so parent process cannot return from the exec call 
	until it knows that the child process has loaded or not. 
	Only the parent process proceed past the sema_down line and return.*/	
	sema_down(&(child_process->load_semaphore));      
	if(child_process->load_success==true)
		return pid;
	else
	{
		return -1;
	}
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

/* write file */
int syscall_write(int fd, void *buffer, unsigned size)
{
	int write_size = 0;
	struct file *current_file;

	if(fd == 1)                    /*stdout */
	{ 
		putbuf((const char *)buffer,size);
		write_size = size;
	}
	else
	{
		current_file = process_get_file(fd);
		if(current_file != NULL)
			write_size = file_write(current_file,(const void *)buffer,size);
	}
	return write_size;
} 

/* References: G,Ko(2015), pintos. Available from: https://github.com/GunjuKo/pintos [accessed on 30/11/22]*/
/****OTHER FUNCTIONS****/
void
check_address(void *addr)
{
	uint32_t address=(unsigned int)addr;

	uint32_t lowest=0x8048000;
	uint32_t highest=0xc0000000;
	if(address >= lowest && address < highest){
		return;
	}
	else{
		syscall_exit(-1);
	}
}

/* References: G,Ko(2015), pintos. Available from: https://github.com/GunjuKo/pintos [accessed on 30/11/22]*/
/* get_argument function */
void
get_argument(void *esp, int *argument, int count)
{
	int i;
	void *stack_ptr=esp+4;
	if(count > 0)
	{
		for(i=0; i<count; i++){
			check_address(stack_ptr);
			argument[i] = *(int *)stack_ptr;
			stack_ptr = stack_ptr + 4;
		}
	}
}