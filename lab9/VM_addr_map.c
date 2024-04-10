#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define MAXSTR 1000

int main(int argc, char *argv[])
{
  char line[MAXSTR];
  int *page_table, *mem_map;
  unsigned int log_size, phy_size, page_size, d;
  unsigned int num_pages, num_frames;
  unsigned int offset, logical_addr, physical_addr, page_num, frame_num;

  /* Get the memory characteristics from the input file */
  fgets(line, MAXSTR, stdin);
  if ((sscanf(line, "Logical address space size: %d^%d", &d, &log_size)) != 2)
  {
    fprintf(stderr, "Unexpected line 1. Abort.\n");
    exit(-1);
  }
  fgets(line, MAXSTR, stdin);
  if ((sscanf(line, "Physical address space size: %d^%d", &d, &phy_size)) != 2)
  {
    fprintf(stderr, "Unexpected line 2. Abort.\n");
    exit(-1);
  }
  fgets(line, MAXSTR, stdin);
  if ((sscanf(line, "Page size: %d^%d", &d, &page_size)) != 2)
  {
    fprintf(stderr, "Unexpected line 3. Abort.\n");
    exit(-1);
  }
  num_pages = 1 << (log_size - page_size);
  num_frames = 1 << (phy_size - page_size);

  fprintf(stdout, "Number of Pages: %d, Number of Frames: %d\n\n", num_pages, num_frames);

  /* Allocate arrays to hold the page table and memory frames map */
  page_table = (int *)malloc(num_pages * sizeof(int));
  mem_map = (int *)malloc(num_frames * sizeof(int));

  /* Initialize page table to indicate that no pages are currently mapped to
     physical memory */
  memset(page_table, 0, num_pages * sizeof(int));

  /* Initialize memory map table to indicate no valid frames */
  memset(mem_map, 0, num_frames * sizeof(int));

  unsigned temp = 0;
  frame_num = 0;

  /* Read each accessed address from input file. Map the logical address to
     corresponding physical address */
  fgets(line, MAXSTR, stdin);
  while (!(feof(stdin)))
  {
    sscanf(line, "0x%x", &logical_addr);
    fprintf(stdout, "Logical Address: 0x%x\n", logical_addr);

    page_num = logical_addr >> page_size;
    offset = logical_addr & ((1 << page_size) - 1);

    fprintf(stdout, "Page Number: %d\n", page_num);

    if (page_table[page_num] == 0)
    {
      printf("Page Fault!\n");
      printf("Frame Number: %d\n", temp);
      mem_map[temp] = offset;
      physical_addr = (temp << page_size) | offset;
      page_table[page_num] = temp + 1;
      temp++;
    }
    else
    {
      frame_num = page_table[page_num];
      frame_num--;
      printf("Frame Number: %d\n", frame_num);
      physical_addr = (frame_num << page_size) | offset;
    }

    fprintf(stdout, "Physical Address: 0x%x\n\n", physical_addr);

    fgets(line, MAXSTR, stdin);
  }

  free(page_table);
  free(mem_map);

  return 0;
}
