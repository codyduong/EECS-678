#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void err_quit (const char * mesg)
{
  printf ("%s\n", mesg);
  exit(1);
}

void err_sys (const char * mesg)
{
  perror(mesg);
  exit(errno);
}

int main (int argc, char *argv[])
{
  int fdin, fdout, bufsz;
  char *src;
  struct stat statbuf;

  if (argc != 4)
    err_quit ("usage: read_write <fromfile> <tofile> <buf_size>");

  /* open the input file */
  if ((fdin = open (argv[1], O_RDONLY)) < 0) {
    char buf[256];
    sprintf(buf, "can't open %s for reading", argv[1]);
    perror(buf);
    exit(errno);
  }

  /* open/create the output file */
  if ((fdout = open (argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0) {
    char buf[256];
    sprintf (buf, "can't create %s for writing", argv[2]);
    perror(buf);
    exit(errno);
  }

  /* Allocate a buffer of the size specified */
  bufsz = atoi(argv[3]);
  src = malloc(bufsz);
  
  /* And use it to copy the file */
  while ((read (fdin, src, bufsz)) > 0) {
    write (fdout, src, bufsz);
  }
} /* main */


