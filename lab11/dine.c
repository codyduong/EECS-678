#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <linux/unistd.h>

#define gettid() syscall(__NR_gettid)

#define NUM_PHILS 5
#define MAX_BUF 256
#define NUM_CHOPS NUM_PHILS
#define FIELDS_TO_IGNORE 13

#define DEADLOCK 0
#define ACTIVE_DURATION 200

typedef struct {
  pthread_t thread;
  pthread_cond_t can_eat; 
  int id;
  int tid;
} philosopher;

/* GLOBALS */
static philosopher diners[NUM_PHILS];
static int stop=0;
static pthread_mutex_t chopstick[NUM_CHOPS];
static unsigned long user_progress[NUM_PHILS];
static unsigned long user_time[NUM_PHILS];
static unsigned long sys_progress[NUM_PHILS];
static unsigned long sys_time[NUM_PHILS];


/*
 * Helper functions for grabbing chopsticks, referencing neighbors
 */
pthread_mutex_t *right_chop (philosopher *p)
{
  return &chopstick[(p->id == 0 ? NUM_CHOPS-1 : (p->id)-1)];
}

pthread_mutex_t *left_chop (philosopher *p)
{
  return &chopstick[p->id];
}

philosopher *left_phil (philosopher *p)
{
  return &diners[(p->id == 0 ? NUM_PHILS-1 : (p->id)-1)];
}

philosopher *right_phil (philosopher *p)
{
  return &diners[(p->id == (NUM_PHILS-1) ? 0 : (p->id)+1)];
}

/*
 * Do a small amount of work that we can use to represent a
 * philosopher thinking one thought
 */
void think_one_thought()
{
  int i, j;
  i = 0;
  i++;
  for (j=0; j<ACTIVE_DURATION; j++) {
    i++;
  }
}

/*
 * Do a small amount of work that we can use to represent a
 * philosopher eating one mouthful of food
 */
void eat_one_mouthful()
{
  int i, j;
  i = 0;
  i++;
  for (j=0; j<ACTIVE_DURATION; j++) {
    i++;
  }
}

/*
 * Philosopher code
 */
static void *dp_thread(void *arg)
{
  int think_rnd;
  int eat_rnd;
  int i;
  philosopher *me;

  me = (philosopher *) arg;
    
  me->tid = gettid();

  while (!stop) {
    think_rnd = (rand() % 10000);
    eat_rnd = (rand() % 10000);

    /*
     * Think a random number of thoughts before getting hungry 
     */
    for (i = 0; i < think_rnd; i++){
      think_one_thought();
    }

    /*
     * Grab both chopsticks
     */
#if DEADLOCK
    /*
     * This order results in deadlock 
     */
    pthread_mutex_lock(left_chop(me));
    pthread_mutex_lock(right_chop(me));
#else
    /*
     * This order avoids deadlock 
     */
    if(me->id % 2 == 0) {
      pthread_mutex_lock(left_chop(me));
      pthread_mutex_lock(right_chop(me));
    } else {
      pthread_mutex_lock(right_chop(me));
      pthread_mutex_lock(left_chop(me));
    }
#endif

    /*
     * Eat some random amount of food
     */
    for (i = 0; i < eat_rnd; i++){
      eat_one_mouthful();
    }

    /*
     * Release both chopsticks
     */
    pthread_mutex_unlock(right_chop(me));
    pthread_mutex_unlock(left_chop(me));
  }

  return NULL;
}

void set_table()
{
  int i;

  for (i = 0; i < NUM_CHOPS; i++) {
    pthread_mutex_init(&chopstick[i], NULL);
  }

  for (i = 0; i < NUM_PHILS; i++) {
    diners[i].id = i;
    diners[i].tid = -1;
    user_progress[i] = 0;
    user_time[i] = 0;
    sys_progress[i] = 0;
    sys_time[i] = 0;
  }

  for (i = 0; i < NUM_PHILS; i++) {
    pthread_create(&(diners[i].thread), NULL, dp_thread, &diners[i]);
  }
    
  /*
   * Stall until the diners initialize their tids 
   */
  i = 0;
  while (i < NUM_PHILS) {
    if(diners[i].tid != -1) i++;
    else sleep(1);
  }
}

void print_progress()
{
  int i;
    
  char buf[MAX_BUF];

  printf ("\nUser time:\t");
  for (i = 0; i < NUM_PHILS; i++) {
    sprintf(buf, "%lu / %lu", user_progress[i], user_time[i]);
    if (strlen(buf) < 8)
      printf("%s\t\t", buf);
    else 
      printf("%s\t", buf);
  }
     
  printf ("\nSystem time:\t");
  for (i = 0; i < NUM_PHILS; i++) {
    sprintf(buf, "%lu / %lu", sys_progress[i], sys_time[i]);
    if (strlen(buf) < 8)
      printf("%s\t\t", buf);
    else 
      printf("%s\t", buf);
  }
     
  printf("\n");
}

int check_for_deadlock()
{
  int   deadlock;
  char  filename[MAX_BUF];
  int   i;
  int   j;
  FILE *statf;

  unsigned long new_sys_time;
  unsigned long new_user_time;

  deadlock = 1;
  for (i = 0; i < NUM_PHILS; i++) {

    /*
     * 1. Store the stat filename for this diner into a buffer. Use the sprintf
     * library call.
     */
    sprintf(filename, "/proc/self/task/%d/stat", diners[i].tid);

    /* 
     * 2. Use fopen to open the stat file as a file stream. Open it
     * with read only permissions.
     */
    statf = fopen(filename, "r");
    if (!statf) {
      perror("failed to open");
      continue;
    }

    /* 
     * 3. Seek over uninteresting fields. Use fscanf to perform the seek.  You
     * also need to determine how many fields to skip over - see proc(5)
     * HINT: Use the the * qualifier to skip tokens without storing them.
     */
    for (j = 0; j < FIELDS_TO_IGNORE; j++) {
      fscanf(statf, "%*s");
    }
    
    /* 
     * 4. Read the time values you want. Use fscanf again. 
     */ 
    fscanf(statf, "%lu %lu", &new_user_time, &new_sys_time);
   
    /*
     * 5. Use time values to determine if deadlock has occurred.
     */
    if (new_user_time > user_time[i] || new_sys_time > sys_time[i]) {
      deadlock = 0;
      user_progress[i] = new_user_time - user_time[i];
      sys_progress[i] = new_sys_time - sys_time[i];
      user_time[i] = new_user_time;
      sys_time[i] = new_sys_time;
    }
 
    /*
     * 6. Close the stat file stream 
     */
    fclose(statf);
  }

  return deadlock;
}


int main(int argc, char **argv)
{
  int i;
  int deadlock;
  deadlock = 0;

  srand(time(NULL));

  set_table();

  do {
    /*
     * Let the philosophers do some thinking and eating
     */
    sleep(5);

    /*
     * Check for deadlock (i.e. none of the philosophers are
     * making progress)
     */
    deadlock = 0;

    if (check_for_deadlock()) {
      deadlock = 1;
      break;
    }

    /*
     * Print out the philosophers progress
     */
    print_progress();
  } while (!deadlock);

  stop = 1;
  printf ("Reached deadlock\n");

  /*
   * Release all locks so philosophers can exit
   */
  for (i = 0; i < NUM_CHOPS; i++)
    pthread_mutex_unlock(&chopstick[i]);

  /*
   * Wait for philosophers to finish
   */
  for (i = 0; i < NUM_PHILS; i++)
    pthread_join(diners[i].thread, NULL);

  return 0;
}

