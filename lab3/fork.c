#include <stdio.h>
#include <unistd.h>

int main()
{
  int pid;

  pid = fork();

  if (pid == 0){
	/*execl("/bin/uname", "uname", "-a", (char *) 0);*/
	printf("child\n");
  } else{
	printf("parent\n");
  }

  printf("%d\n", pid);

  return 0;
}
