/*
 ===============================================================================
 Exercise: 10_thread_pool.c
 Description: Thread Pool Pattern for Parallel Processing
 Platform: GNU/Linux (Arch/WSL) on x86_64
 Compile:  gcc 10_thread_pool.c -o thread_pool -pthread
 ===============================================================================
 Features:
 - Fixed-size Thread Pool with configurable workers
 - Thread-safe Task Queue using Mutex
 - Condition Variables for efficient sleep/wake (pthread_cond_t)
 - Graceful shutdown mechanism with broadcast
 - Factorial and Fibonacci task types
 - Interactive menu for repeated demos
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
#define NUM_WORKERS 4
#define MAX_QUEUE 32
#define MIN_OPTION 1
#define MAX_OPTION 4

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_THREAD_CREATE,
  ERR_QUEUE_FULL
} Status;

typedef enum { TASK_FACTORIAL, TASK_FIBONACCI } TaskType;

typedef struct {
  int id;
  TaskType type;
  int value;
} Task;

typedef struct {
  Task tasks[MAX_QUEUE];
  int head;
  int tail;
  int count;

  int active_workers;
  int shutdown;
  int tasks_completed;

  pthread_mutex_t lock;
  pthread_cond_t cond_task_available;
  pthread_cond_t cond_all_done;
} ThreadPool;

void show_menu(void);
void handle_error(Status status);

void run_factorial_pool(void);
void run_mixed_pool(void);
void run_pool_info(void);

void clear_input_buffer(void);
Status read_integer(int *value);

void pool_init(ThreadPool *pool);
Status pool_submit(ThreadPool *pool, int id, TaskType type, int value);
void pool_wait(ThreadPool *pool);
void pool_shutdown(ThreadPool *pool, pthread_t *workers, int num_workers);

void *worker_routine(void *arg);
unsigned long long compute_factorial(int n);
long long compute_fibonacci(int n);
pid_t gettid_helper(void);

/* Worker needs access to the pool */
ThreadPool g_pool;

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
      run_factorial_pool();
      break;
    case 2:
      run_mixed_pool();
      break;
    case 3:
      run_pool_info();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Thread Pool Pattern ===\n\n");
  printf("Workers: %d | Queue Capacity: %d\n\n", NUM_WORKERS, MAX_QUEUE);
  printf("1. Factorial Tasks (%d tasks)\n", 10);
  printf("2. Mixed Tasks (Factorial + Fibonacci)\n");
  printf("3. Thread Pool Concepts\n");
  printf("4. Exit\n");
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
    printf("Error: Failed to create worker thread.\n\n");
    break;
  case ERR_QUEUE_FULL:
    printf("Error: Task queue is full.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

pid_t gettid_helper(void) { return syscall(SYS_gettid); }

unsigned long long compute_factorial(int n) {
  unsigned long long f = 1;
  for (int i = 2; i <= n; i++) {
    f *= i;
  }
  return f;
}

long long compute_fibonacci(int n) {
  if (n <= 1)
    return n;
  long long a = 0, b = 1;
  for (int i = 2; i <= n; i++) {
    long long temp = a + b;
    a = b;
    b = temp;
  }
  return b;
}

void pool_init(ThreadPool *pool) {
  pool->head = 0;
  pool->tail = 0;
  pool->count = 0;
  pool->active_workers = 0;
  pool->shutdown = 0;
  pool->tasks_completed = 0;
  pthread_mutex_init(&pool->lock, NULL);
  pthread_cond_init(&pool->cond_task_available, NULL);
  pthread_cond_init(&pool->cond_all_done, NULL);
}

Status pool_submit(ThreadPool *pool, int id, TaskType type, int value) {
  pthread_mutex_lock(&pool->lock);

  if (pool->count >= MAX_QUEUE) {
    pthread_mutex_unlock(&pool->lock);
    return ERR_QUEUE_FULL;
  }

  pool->tasks[pool->tail].id = id;
  pool->tasks[pool->tail].type = type;
  pool->tasks[pool->tail].value = value;
  pool->tail = (pool->tail + 1) % MAX_QUEUE;
  pool->count++;

  pthread_cond_signal(&pool->cond_task_available);
  pthread_mutex_unlock(&pool->lock);

  return SUCCESS;
}

void pool_wait(ThreadPool *pool) {
  pthread_mutex_lock(&pool->lock);
  while (pool->count > 0 || pool->active_workers > 0) {
    pthread_cond_wait(&pool->cond_all_done, &pool->lock);
  }
  pthread_mutex_unlock(&pool->lock);
}

void pool_shutdown(ThreadPool *pool, pthread_t *workers, int num_workers) {
  printf("  Shutting down pool...\n");

  pthread_mutex_lock(&pool->lock);
  pool->shutdown = 1;
  pthread_cond_broadcast(&pool->cond_task_available);
  pthread_mutex_unlock(&pool->lock);

  for (int i = 0; i < num_workers; i++) {
    pthread_join(workers[i], NULL);
  }

  pthread_mutex_destroy(&pool->lock);
  pthread_cond_destroy(&pool->cond_task_available);
  pthread_cond_destroy(&pool->cond_all_done);
}

void *worker_routine(void *arg) {
  int worker_id = *(int *)arg;
  ThreadPool *pool = &g_pool;

  printf("  [WORKER #%d] Started (TID: %d)\n", worker_id, gettid_helper());

  while (TRUE) {
    pthread_mutex_lock(&pool->lock);

    while (pool->count == 0 && !pool->shutdown) {
      pthread_cond_wait(&pool->cond_task_available, &pool->lock);
    }

    if (pool->shutdown && pool->count == 0) {
      pthread_mutex_unlock(&pool->lock);
      break;
    }

    // Dequeue task
    Task task = pool->tasks[pool->head];
    pool->head = (pool->head + 1) % MAX_QUEUE;
    pool->count--;
    pool->active_workers++;

    pthread_mutex_unlock(&pool->lock);

    // Process task (outside critical section)
    if (task.type == TASK_FACTORIAL) {
      unsigned long long result = compute_factorial(task.value);
      printf("  [WORKER #%d] Task %d: factorial(%d) = %llu\n", worker_id,
             task.id, task.value, result);
    } else {
      long long result = compute_fibonacci(task.value);
      printf("  [WORKER #%d] Task %d: fibonacci(%d) = %lld\n", worker_id,
             task.id, task.value, result);
    }

    usleep(15000); // Simulate processing time

    // Mark completion
    pthread_mutex_lock(&pool->lock);
    pool->active_workers--;
    pool->tasks_completed++;

    if (pool->count == 0 && pool->active_workers == 0) {
      pthread_cond_signal(&pool->cond_all_done);
    }
    pthread_mutex_unlock(&pool->lock);
  }

  printf("  [WORKER #%d] Terminated.\n", worker_id);
  return NULL;
}

void run_factorial_pool(void) {
  int num_tasks = 10;
  int task_values[] = {10, 12, 8, 15, 9, 6, 11, 7, 14, 20};

  printf("\n=== 1. Factorial Task Pool ===\n");
  printf("  %d workers processing %d factorial tasks.\n\n", NUM_WORKERS,
         num_tasks);

  pool_init(&g_pool);

  pthread_t workers[NUM_WORKERS];
  int worker_ids[NUM_WORKERS];

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  for (int i = 0; i < NUM_WORKERS; i++) {
    worker_ids[i] = i + 1;
    if (pthread_create(&workers[i], NULL, worker_routine, &worker_ids[i]) !=
        0) {
      handle_error(ERR_THREAD_CREATE);
      return;
    }
  }

  usleep(10000); // Let workers print startup messages
  printf("\n  Submitting %d tasks...\n\n", num_tasks);

  for (int i = 0; i < num_tasks; i++) {
    pool_submit(&g_pool, i + 1, TASK_FACTORIAL, task_values[i]);
  }

  pool_wait(&g_pool);

  clock_gettime(CLOCK_MONOTONIC, &end);
  double elapsed =
      (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  printf("\n  - Tasks processed: %d\n", g_pool.tasks_completed);
  printf("  - Elapsed time:   %.3f seconds\n", elapsed);
  printf("  - Avg per task:    %.3f seconds\n\n", elapsed / num_tasks);

  pool_shutdown(&g_pool, workers, NUM_WORKERS);
  printf("\n");
}

void run_mixed_pool(void) {
  printf("\n=== 2. Mixed Task Pool ===\n");
  printf("  %d workers processing factorial + fibonacci tasks.\n\n",
         NUM_WORKERS);

  pool_init(&g_pool);

  pthread_t workers[NUM_WORKERS];
  int worker_ids[NUM_WORKERS];

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  for (int i = 0; i < NUM_WORKERS; i++) {
    worker_ids[i] = i + 1;
    if (pthread_create(&workers[i], NULL, worker_routine, &worker_ids[i]) !=
        0) {
      handle_error(ERR_THREAD_CREATE);
      return;
    }
  }

  usleep(10000);
  printf("\n  Submitting mixed tasks...\n\n");

  pool_submit(&g_pool, 1, TASK_FACTORIAL, 10);
  pool_submit(&g_pool, 2, TASK_FIBONACCI, 30);
  pool_submit(&g_pool, 3, TASK_FACTORIAL, 15);
  pool_submit(&g_pool, 4, TASK_FIBONACCI, 40);
  pool_submit(&g_pool, 5, TASK_FACTORIAL, 20);
  pool_submit(&g_pool, 6, TASK_FIBONACCI, 50);
  pool_submit(&g_pool, 7, TASK_FACTORIAL, 12);
  pool_submit(&g_pool, 8, TASK_FIBONACCI, 35);

  pool_wait(&g_pool);

  clock_gettime(CLOCK_MONOTONIC, &end);
  double elapsed =
      (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  printf("\n  - Tasks processed: %d\n", g_pool.tasks_completed);
  printf("  - Elapsed time:   %.3f seconds\n\n", elapsed);

  pool_shutdown(&g_pool, workers, NUM_WORKERS);
  printf("\n");
}

void run_pool_info(void) {
  printf("\n=== 3. Thread Pool Concepts ===\n\n");
  printf("  Architecture:\n");
  printf("  MAIN ──submit()──> [TASK QUEUE] ──dequeue()──> WORKER THREADS\n\n");

  printf("  Components:\n");
  printf("  - Task Queue:   Circular buffer protected by mutex.\n");
  printf("  - Workers:      Threads sleeping on cond_wait until task "
         "arrives.\n");
  printf("  - Mutex:        Protects queue head/tail/count.\n");
  printf("  - Cond Vars:    cond_task_available (wake workers),\n");
  printf("                  cond_all_done (wake main).\n\n");

  printf("  Lifecycle:\n");
  printf("  1. pool_init()     Initialize queue, mutex, cond vars.\n");
  printf("  2. Create workers  pthread_create() with worker_routine.\n");
  printf("  3. pool_submit()   Enqueue tasks, signal workers.\n");
  printf("  4. pool_wait()     Main sleeps until all tasks done.\n");
  printf("  5. pool_shutdown() Set flag, broadcast, join all workers.\n\n");

  printf("  Benefits:\n");
  printf("  - Reuse threads (avoid create/destroy overhead).\n");
  printf("  - Control max concurrency (fixed pool size).\n");
  printf("  - Clean shutdown with no orphaned threads.\n\n");
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
