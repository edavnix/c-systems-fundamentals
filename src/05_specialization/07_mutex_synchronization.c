/*
 ===============================================================================
 Exercise: 07_mutex_synchronization.c
 Description: Multithreading Synchronization using Mutexes (Race Conditions)
 Platform: GNU/Linux (Arch/WSL) on x86_64
 Compile:  gcc 07_mutex_synchronization.c -o mutex_sync -pthread
 ===============================================================================
 Features:
 - Deliberate creation of a Race Condition (read-modify-write split)
 - Critical section protection using pthread_mutex_t
 - Mutex initialization, lock, and unlock mechanisms
 - Side-by-side comparison of unsafe vs safe execution
 - Interactive menu for repeated demos
 ===============================================================================
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define NUM_THREADS 5
#define ITERATIONS 1000
#define MIN_OPTION 1
#define MAX_OPTION 5

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_THREAD_CREATE
} Status;

int shared_counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void show_menu(void);
void handle_error(Status status);

void run_race_condition(void);
void run_mutex_protected(void);
void run_side_by_side(void);
void run_concepts(void);

void clear_input_buffer(void);
Status read_integer(int *value);

void *task_no_mutex(void *arg);
void *task_with_mutex(void *arg);

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
      pthread_mutex_destroy(&mutex);
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    switch (option) {
    case 1:
      run_race_condition();
      break;
    case 2:
      run_mutex_protected();
      break;
    case 3:
      run_side_by_side();
      break;
    case 4:
      run_concepts();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Mutex Synchronization ===\n\n");
  printf("Threads: %d | Iterations per thread: %d\n", NUM_THREADS, ITERATIONS);
  printf("Expected result: %d\n\n", NUM_THREADS * ITERATIONS);
  printf("1. Run WITHOUT Mutex (Race Condition)\n");
  printf("2. Run WITH Mutex (Protected)\n");
  printf("3. Side-by-Side Comparison\n");
  printf("4. Mutex Concepts & Theory\n");
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

void *task_no_mutex(void *arg) {
  int id = *(int *)arg;
  for (int i = 0; i < ITERATIONS; i++) {
    int temp = shared_counter;
    usleep(1); // Force context switch to amplify race condition
    shared_counter = temp + 1;
  }
  printf("  [THREAD #%d] Done (no mutex).\n", id);
  return NULL;
}

void *task_with_mutex(void *arg) {
  int id = *(int *)arg;
  for (int i = 0; i < ITERATIONS; i++) {
    pthread_mutex_lock(&mutex);
    int temp = shared_counter;
    usleep(1); // Even with delay, mutex protects the entire block
    shared_counter = temp + 1;
    pthread_mutex_unlock(&mutex);
  }
  printf("  [THREAD #%d] Done (with mutex).\n", id);
  return NULL;
}

void run_race_condition(void) {
  pthread_t threads[NUM_THREADS];
  int ids[NUM_THREADS];

  printf("\n=== 1. Execution WITHOUT Mutex ===\n");
  printf("  Starting %d threads, each incrementing %d times...\n\n",
         NUM_THREADS, ITERATIONS);

  shared_counter = 0;

  for (int i = 0; i < NUM_THREADS; i++) {
    ids[i] = i + 1;
    if (pthread_create(&threads[i], NULL, task_no_mutex, &ids[i]) != 0) {
      handle_error(ERR_THREAD_CREATE);
      return;
    }
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  int expected = NUM_THREADS * ITERATIONS;
  int lost = expected - shared_counter;

  printf("\n  - Expected: %d\n", expected);
  printf("  - Actual:   %d\n", shared_counter);

  if (lost > 0) {
    printf("  - ⚠ RACE CONDITION: %d increments lost! (%.1f%% data loss)\n\n",
           lost, (double)lost / expected * 100.0);
  } else {
    printf("  - ✓ No loss this run (but result is NOT guaranteed).\n\n");
  }
}

void run_mutex_protected(void) {
  pthread_t threads[NUM_THREADS];
  int ids[NUM_THREADS];

  printf("\n=== 2. Execution WITH Mutex ===\n");
  printf("  Starting %d threads with mutex protection...\n\n", NUM_THREADS);

  shared_counter = 0;

  for (int i = 0; i < NUM_THREADS; i++) {
    ids[i] = i + 1;
    if (pthread_create(&threads[i], NULL, task_with_mutex, &ids[i]) != 0) {
      handle_error(ERR_THREAD_CREATE);
      return;
    }
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  int expected = NUM_THREADS * ITERATIONS;

  printf("\n  - Expected: %d\n", expected);
  printf("  - Actual:   %d\n", shared_counter);

  if (shared_counter == expected) {
    printf("  - ✓ Mutex guaranteed mutual exclusion. Zero data loss.\n\n");
  } else {
    printf("  - ⚠ Unexpected result.\n\n");
  }
}

void run_side_by_side(void) {
  printf("\n=== 3. Side-by-Side Comparison ===\n\n");

  // Phase 1: No Mutex
  pthread_t threads[NUM_THREADS];
  int ids[NUM_THREADS];
  int expected = NUM_THREADS * ITERATIONS;

  shared_counter = 0;
  printf("  [Phase 1] Without Mutex...\n");
  for (int i = 0; i < NUM_THREADS; i++) {
    ids[i] = i + 1;
    pthread_create(&threads[i], NULL, task_no_mutex, &ids[i]);
  }
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  int result_unsafe = shared_counter;

  // Phase 2: With Mutex
  shared_counter = 0;
  printf("\n  [Phase 2] With Mutex...\n");
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], NULL, task_with_mutex, &ids[i]);
  }
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  int result_safe = shared_counter;

  // Results table
  printf("\n  %-20s | %-10s | %-10s\n", "Metric", "No Mutex", "With Mutex");
  printf("  ---------------------|------------|----------\n");
  printf("  %-20s | %-10d | %-10d\n", "Expected", expected, expected);
  printf("  %-20s | %-10d | %-10d\n", "Actual", result_unsafe, result_safe);
  printf("  %-20s | %-10d | %-10d\n", "Lost Increments",
         expected - result_unsafe, expected - result_safe);
  printf("  %-20s | %-10s | %-10s\n", "Status",
         (result_unsafe != expected) ? "⚠ CORRUPT" : "✓ OK",
         (result_safe == expected) ? "✓ OK" : "⚠ CORRUPT");
  printf("\n");
}

void run_concepts(void) {
  printf("\n=== 4. Mutex Concepts ===\n\n");
  printf("  What is a Mutex?\n");
  printf("  - Mutual Exclusion lock for shared resources.\n");
  printf("  - Only one thread can hold the lock at a time.\n");
  printf("  - Others block (wait) until it's released.\n\n");

  printf("  API Reference:\n");
  printf("  - pthread_mutex_init(&m, NULL)  Initialize mutex.\n");
  printf("  - pthread_mutex_lock(&m)        Acquire (blocks if held).\n");
  printf("  - pthread_mutex_unlock(&m)      Release.\n");
  printf("  - pthread_mutex_destroy(&m)     Clean up resources.\n\n");

  printf("  Critical Section Pattern:\n");
  printf("    pthread_mutex_lock(&mutex);\n");
  printf("    // ... shared data access ...\n");
  printf("    pthread_mutex_unlock(&mutex);\n\n");

  printf("  Common Pitfalls:\n");
  printf("  - Forgetting to unlock -> Deadlock.\n");
  printf("  - Locking in wrong order -> Deadlock (with multiple mutexes).\n");
  printf("  - Over-locking -> Performance bottleneck.\n\n");

  printf("  Applications:\n");
  printf("  - Database access, shared counters, data structures.\n");
  printf("  - Producer-consumer queues, thread-safe logging.\n\n");
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
