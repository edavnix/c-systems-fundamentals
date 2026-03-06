/*
 ===============================================================================
 Exercise: 06_pthread_basics.c
 Description: Multithreading with POSIX Threads (pthreads)
 Platform: GNU/Linux (Arch/WSL) on x86_64
 Compile:  gcc 06_pthread_basics.c -o pthread_basics -pthread
 ===============================================================================
 Features:
 - Thread creation (pthread_create) and synchronization (pthread_join)
 - OS-level Thread ID (TID) inspection
 - Concurrent execution and out-of-order completion
 - Shared memory demonstration between threads
 - Mutex synchronization for race condition prevention
 - Interactive menu for multiple demos
 ===============================================================================
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define NUM_THREADS 4
#define MIN_OPTION 1
#define MAX_OPTION 5

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_THREAD_CREATE
} Status;

typedef struct {
  int id;
  long result;
} ThreadData;

/* Shared counter for race condition demo */
int shared_counter = 0;
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void show_menu(void);
void handle_error(Status status);

void run_basic_threads(void);
void run_shared_memory_demo(void);
void run_mutex_demo(void);
void run_concepts(void);

void clear_input_buffer(void);
Status read_integer(int *value);

pid_t gettid_helper(void);
void *task_compute(void *arg);
void *task_unsafe_increment(void *arg);
void *task_safe_increment(void *arg);

int main(void) {
  int option = 0;

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nExiting. Goodbye!\n");
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    switch (option) {
    case 1:
      run_basic_threads();
      break;
    case 2:
      run_shared_memory_demo();
      break;
    case 3:
      run_mutex_demo();
      break;
    case 4:
      run_concepts();
      break;
    }
  }

  pthread_mutex_destroy(&counter_mutex);
  return 0;
}

void show_menu(void) {
  printf("=== POSIX Threads (pthreads) ===\n\n");
  printf("PID: %d\n\n", getpid());
  printf("1. Basic Thread Create + Join\n");
  printf("2. Shared Memory Demo (Race Condition)\n");
  printf("3. Mutex Synchronization Demo\n");
  printf("4. Threads vs Processes Concepts\n");
  printf("5. Exit\n");
  printf("Option: ");
}

void handle_error(Status status) {
  switch (status) {
  case ERR_INVALID_INPUT:
    printf("Error: Invalid input. Please enter a valid number.\n\n");
    break;
  case ERR_INVALID_OPTION:
    printf("Error: Invalid option selected.\n\n");
    break;
  case ERR_THREAD_CREATE:
    printf("Error: Failed to create thread.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

pid_t gettid_helper(void) { return syscall(SYS_gettid); }

void *task_compute(void *arg) {
  ThreadData *data = (ThreadData *)arg;
  pid_t tid = gettid_helper();

  printf("  [THREAD #%d TID:%d] Started. Computing sum 1..1000\n", data->id,
         tid);

  long sum = 0;
  for (int i = 1; i <= 1000; i++) {
    sum += i;
  }

  // Simulate variable completion times
  usleep((rand() % 50) * 1000);

  data->result = sum;
  printf("  [THREAD #%d TID:%d] Result: %ld\n", data->id, tid, sum);

  return NULL;
}

void *task_unsafe_increment(void *arg) {
  int id = *(int *)arg;
  for (int i = 0; i < 100000; i++) {
    shared_counter++; // NOT atomic - race condition!
  }
  printf("  [THREAD #%d] Finished 100,000 increments (no mutex).\n", id);
  return NULL;
}

void *task_safe_increment(void *arg) {
  int id = *(int *)arg;
  for (int i = 0; i < 100000; i++) {
    pthread_mutex_lock(&counter_mutex);
    shared_counter++;
    pthread_mutex_unlock(&counter_mutex);
  }
  printf("  [THREAD #%d] Finished 100,000 increments (with mutex).\n", id);
  return NULL;
}

void run_basic_threads(void) {
  pthread_t threads[NUM_THREADS];
  ThreadData data[NUM_THREADS];

  printf("\n=== 1. Basic Thread Create + Join ===\n");
  printf("  Main Thread PID: %d, TID: %d\n", getpid(), gettid_helper());
  printf("  Creating %d worker threads...\n\n", NUM_THREADS);

  srand(time(NULL));

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  for (int i = 0; i < NUM_THREADS; i++) {
    data[i].id = i + 1;
    data[i].result = 0;

    if (pthread_create(&threads[i], NULL, task_compute, &data[i]) != 0) {
      handle_error(ERR_THREAD_CREATE);
      return;
    }
  }

  usleep(5000);
  printf("  [MAIN] Waiting for all threads (pthread_join)...\n");

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
    printf("  [MAIN] Thread #%d joined (result: %ld).\n", i + 1,
           data[i].result);
  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  double elapsed =
      (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  printf("\n  - Threads created:  %d\n", NUM_THREADS);
  printf("  - All completed OK.\n");
  printf("  - Elapsed time:    %.4f seconds\n\n", elapsed);
}

void run_shared_memory_demo(void) {
  printf("\n=== 2. Shared Memory Demo (Race Condition) ===\n");
  printf("  %d threads each increment a shared counter 100,000 times.\n",
         NUM_THREADS);
  printf("  Expected result: %d\n\n", NUM_THREADS * 100000);

  shared_counter = 0;

  pthread_t threads[NUM_THREADS];
  int ids[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++) {
    ids[i] = i + 1;
    if (pthread_create(&threads[i], NULL, task_unsafe_increment, &ids[i]) !=
        0) {
      handle_error(ERR_THREAD_CREATE);
      return;
    }
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("\n  - Expected: %d\n", NUM_THREADS * 100000);
  printf("  - Actual:   %d\n", shared_counter);

  if (shared_counter != NUM_THREADS * 100000) {
    printf("  - ⚠ RACE CONDITION! Data was corrupted.\n");
    printf("  - Lost increments: %d\n", (NUM_THREADS * 100000) - shared_counter);
  } else {
    printf("  - ✓ No race condition this run (but it's not guaranteed).\n");
  }
  printf("\n");
}

void run_mutex_demo(void) {
  printf("\n=== 3. Mutex Synchronization Demo ===\n");
  printf("  Same test, but with pthread_mutex_lock/unlock.\n\n");

  shared_counter = 0;

  pthread_t threads[NUM_THREADS];
  int ids[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++) {
    ids[i] = i + 1;
    if (pthread_create(&threads[i], NULL, task_safe_increment, &ids[i]) != 0) {
      handle_error(ERR_THREAD_CREATE);
      return;
    }
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("\n  - Expected: %d\n", NUM_THREADS * 100000);
  printf("  - Actual:   %d\n", shared_counter);

  if (shared_counter == NUM_THREADS * 100000) {
    printf("  - ✓ Correct! Mutex prevented race condition.\n\n");
  } else {
    printf("  - ⚠ Unexpected result.\n\n");
  }
}

void run_concepts(void) {
  printf("\n=== 4. Threads vs Processes ===\n\n");
  printf("  THREADS:\n");
  printf("  - Share same address space (heap, globals).\n");
  printf("  - Lower creation overhead (no memory copy).\n");
  printf("  - Faster inter-thread communication.\n");
  printf("  - Risk: Race conditions require synchronization.\n\n");
  printf("  PROCESSES (fork):\n");
  printf("  - Separate memory spaces (copy-on-write).\n");
  printf("  - Stronger isolation (crash doesn't affect parent).\n");
  printf("  - IPC needed for communication (pipes, sockets).\n");
  printf("  - Higher overhead per creation.\n\n");
  printf("  Key APIs:\n");
  printf("  - pthread_create():  Spawn a new thread.\n");
  printf("  - pthread_join():    Wait for thread completion.\n");
  printf("  - pthread_mutex_*:   Mutual exclusion for shared data.\n");
  printf("  - Compile with:      gcc ... -pthread\n\n");
}

void clear_input_buffer(void) {
  int c;
  while ((c = getchar()) != '\n' && c != EOF) {
    ;
  }
}

Status read_integer(int *value) {
  if (scanf("%d", value) != 1) {
    clear_input_buffer();
    return ERR_INVALID_INPUT;
  }
  clear_input_buffer();
  return SUCCESS;
}
