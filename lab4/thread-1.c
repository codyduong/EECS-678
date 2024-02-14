#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void *child_fn(void *param);

int global; /* global variable */

int main (int argc, char *argv[])
{
  int local;
  int ret;
  
  pthread_t tid; /* thread identifier */
  pthread_attr_t attr; /* set of thread attributes */
  
  /* update local and global variables */
  global = 100;
  local = 678;

  printf("Original values, global = %d, local = %d\n", global, local);
  
  /* create a new process */
  ret = fork();  
  if(ret == 0){ // Child process
    child_fn(&local);
    exit(0);
  }
  else{ // Parent process

    wait(NULL);

    /* Question 1: Are changes made to the local or global variables by the
       child process reflected in the parent process? Explain. */
    printf("After fork, global = %d, local = %d\n", global, local);
  }
       
  /* get the default thread attributes */
  pthread_attr_init(&attr);
  /* create the thread */
  pthread_create(&tid, &attr, child_fn, &local);
  /* wait for the thread to exit */
  pthread_join(tid, NULL);

  /* Question 2: Are changes made to the local or global variables by the
     child thread reflected in the parent process?
     Separately explain what happens for the local and global variables. */
  printf("After thread, global = %d local = %d\n", global, local);
}

/* Function called by the child process/thread */
void *child_fn(void *param)
{
  int *val = (int*)(param);

  global = 678;
  *val = 100;
}
