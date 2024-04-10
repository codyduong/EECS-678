/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"

/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements.
*/
typedef struct _job_t
{
  int job_number;
  int arrival_time;
  int running_time;
  int remaining_time;
  int priority;
  int start_time;
  int end_time;
  int started;
} job_t;

typedef struct _core_t
{
  int is_idle;
  job_t *job;
} core_t;

core_t *core_array;
int total_cores;
scheme_t scheduling_scheme;
priqueue_t job_queue;

int total_jobs_completed = 0;
float total_waiting_time = 0;
float total_turnaround_time = 0;
float total_response_time = 0;

typedef int(comparer)(const void *, const void *);

int compare_fcfs(const void *a, const void *b)
{
  const job_t *job_a = (const job_t *)a;
  const job_t *job_b = (const job_t *)b;
  return job_a->arrival_time - job_b->arrival_time;
}

int compare_sjf_psjf(const void *a, const void *b)
{
  const job_t *job_a = (const job_t *)a;
  const job_t *job_b = (const job_t *)b;
  return job_a->running_time - job_b->running_time;
}

int compare_pri_ppri(const void *a, const void *b)
{
  const job_t *job_a = (const job_t *)a;
  const job_t *job_b = (const job_t *)b;
  if (job_a->priority == job_b->priority)
    return job_a->arrival_time - job_b->arrival_time; // tie-breaker
  return job_a->priority - job_b->priority;
}

int compare_rr(const void *a, const void *b)
{
  return 0;
}

// ok this is dumb, and was written before globalizec scheme set in startup
// but w/e
int (*job_comparer(scheme_t scheme))(const void *a, const void *b)
{
  switch (scheme)
  {
  case FCFS:
    return compare_fcfs;
  case SJF:
  case PSJF:
    return compare_sjf_psjf;
  case PRI:
  case PPRI:
    return compare_pri_ppri;
  case RR:
    // this is handled by the simulator when our quantum slice expires
    return compare_rr;
  default:
    return compare_fcfs; // Default to FCFS
  }
}

/**
  Initalizes the scheduler.

  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
  total_cores = cores;
  scheduling_scheme = scheme;

  core_array = malloc(sizeof(core_t) * cores);
  for (int i = 0; i < cores; i++)
  {
    core_array[i].is_idle = 1;
    core_array[i].job = NULL;
  }

  priqueue_init(&job_queue, job_comparer(scheme));
}

/**
  Called when a new job arrives.

  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumption:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made.

 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
  job_t *new_job = (job_t *)malloc(sizeof(job_t));
  *new_job = (job_t){
      .job_number = job_number,
      .arrival_time = time,
      .running_time = running_time,
      .remaining_time = running_time,
      .priority = priority,
      .start_time = -1,
      .end_time = 0,
      .started = 0};

  for (int i = 0; i < total_cores; i++)
  {
    if (core_array[i].is_idle)
    {
      core_array[i].is_idle = 0;
      core_array[i].job = new_job;
      new_job->start_time = time;
      new_job->started = 1;
      return i;
    }
  }

  int preempt_core_id = -1;
  for (int i = 0; i < total_cores; i++)
  {
    job_t *current_job = core_array[i].job;
    if (current_job != NULL && ((scheduling_scheme == PPRI && priority < current_job->priority) ||
                                (scheduling_scheme == PSJF && running_time < current_job->remaining_time)))
    {
      preempt_core_id = i;
      break;
    }
  }

  if (preempt_core_id != -1)
  {
    job_t *preempted_job = core_array[preempt_core_id].job;
    preempted_job->remaining_time -= (time - preempted_job->start_time);
    priqueue_offer(&job_queue, preempted_job);

    core_array[preempt_core_id].job = new_job;
    new_job->start_time = time;
    new_job->started = 1;
    return preempt_core_id;
  }

  priqueue_offer(&job_queue, new_job);
  return -1; // Job is queued
}

/**
  Called when a job has completed execution.

  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.

  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
  if (!core_array[core_id].is_idle && core_array[core_id].job)
  {
    job_t *finished_job = core_array[core_id].job;
    finished_job->end_time = time;
    int waiting_time = finished_job->end_time - finished_job->arrival_time - finished_job->running_time;
    int turnaround_time = finished_job->end_time - finished_job->arrival_time;
    int response_time = finished_job->start_time - finished_job->arrival_time;

    total_waiting_time += waiting_time;
    total_turnaround_time += turnaround_time;
    if (finished_job->started)
    {
      total_response_time += response_time;
      total_jobs_completed++;
    }

    free(finished_job);
    core_array[core_id].is_idle = 1;
    core_array[core_id].job = NULL;
  }

  if (priqueue_size(&job_queue) > 0)
  {
    job_t *next_job = (job_t *)priqueue_poll(&job_queue);
    core_array[core_id].is_idle = 0;
    core_array[core_id].job = next_job;
    if (next_job->start_time == -1)
    {
      next_job->start_time = time;
      next_job->started = 1;
    }
    return next_job->job_number;
  }

  return -1;
}

/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.

  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
  if (core_array[core_id].is_idle == 0)
  {
    job_t *current_job = core_array[core_id].job;
    priqueue_offer(&job_queue, current_job);
    core_array[core_id].job = NULL;
    core_array[core_id].is_idle = 1;
  }

  if (priqueue_size(&job_queue) > 0)
  {
    job_t *next_job = (job_t *)priqueue_poll(&job_queue);
    core_array[core_id].is_idle = 0;
    core_array[core_id].job = next_job;
    if (next_job->start_time == -1)
    {
      next_job->start_time = time;
    }
    return next_job->job_number;
  }

  return -1; // No job scheduled
}

/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
  if (total_jobs_completed == 0)
    return 0.0;
  return total_waiting_time / total_jobs_completed;
}

/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
  if (total_jobs_completed == 0)
    return 0;
  return total_turnaround_time / total_jobs_completed;
}

/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
  if (total_jobs_completed == 0)
    return 0.0;
  return total_response_time / total_jobs_completed;
}

/**
  Free any memory associated with your scheduler.

  Assumption:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
  while (priqueue_size(&job_queue) > 0)
  {
    job_t *job = (job_t *)priqueue_poll(&job_queue);
    free(job);
  }

  priqueue_destroy(&job_queue);
  free(core_array);
}

/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)

  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{
}
