/*
 * fork.c: Program to learn about fork/exec system calls
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
  pid_t ret;
  int status;

  /* Create a new process */
  ret = fork();

  // Question 1: Which process prints this line? What is printed?
  printf("After fork, Process id = %ld\n", ret);

  if(ret < 0){
    /* if fork fails */
    fprintf(stderr, "fork failed\n");
    exit(-1);
  }
  else if(ret == 0){
    /* This block is only reached by the child process */

    // Question 4: What happens if the parent process is killed first? Uncomment the next two lines.
    //sleep(4);
    //printf("In Child: %d, Parent: %d\n", getpid(), getppid());
    
    // Question 2: What will be printed if this like is commented?
    execlp("/bin/ls", "-l", NULL);

    // Question 3: When is this line reached/printed?
    fprintf(stderr, "print after execlp\n");
  }
  else {
    printf("In Parent: %d, Child id: %d\n", getpid(), ret);
    // wait(&status);
  }

  printf("Final statement from Process: %d\n", getpid());
}
