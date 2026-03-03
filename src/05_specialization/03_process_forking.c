/*
 ===============================================================================
 Exercise: 03_process_forking.c
 Description: Process Creation and Management (fork, wait, memory isolation)
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Process cloning using fork()
 - Parent/Child logic differentiation
 - Concurrent task execution simulation
 - Process synchronization using wait()
 - Memory isolation demonstration between processes
 - Interactive menu for multiple demos
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 4
#define NUM_CHILDREN 3

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_FORK_FAILED
} Status;

void show_menu(void);
void handle_error(Status status);

void run_basic_fork(void);
void run_multi_fork(void);
void run_memory_isolation(void);

void clear_input_buffer(void);
Status read_integer(int *value);

void child_task(int id);

int main(void) {
  int option = 0;
  srand(getpid());

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
      run_basic_fork();
      break;
    case 2:
      run_multi_fork();
      break;
    case 3:
      run_memory_isolation();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Process Forking Demos ===\n\n");
  printf("1. Basic Fork (Single Child)\n");
  printf("2. Multi-Fork (%d Children + Wait)\n", NUM_CHILDREN);
  printf("3. Memory Isolation Demo\n");
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
  case ERR_FORK_FAILED:
    printf("Error: fork() system call failed.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_basic_fork(void) {
  printf("\n=== 1. Basic Fork ===\n");
  printf("  Parent PID: %d\n", getpid());
  printf("  Calling fork()...\n\n");

  pid_t pid = fork();

  if (pid < 0) {
    handle_error(ERR_FORK_FAILED);
    return;
  } else if (pid == 0) {
    // Child process
    printf("  [CHILD]  PID: %d | Parent PID: %d\n", getpid(), getppid());
    printf("  [CHILD]  I am the cloned process.\n");
    printf("  [CHILD]  fork() returned 0 to me.\n");
    exit(0);
  } else {
    // Parent process
    printf("  [PARENT] PID: %d | Child PID: %d\n", getpid(), pid);
    printf("  [PARENT] fork() returned child's PID (%d) to me.\n", pid);

    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
      printf("  [PARENT] Child exited with code: %d\n\n", WEXITSTATUS(status));
    }
  }
}

void run_multi_fork(void) {
  printf("\n=== 2. Multi-Fork (%d Children) ===\n", NUM_CHILDREN);
  printf("  Parent PID: %d\n", getpid());
  printf("  Creating %d child processes...\n\n", NUM_CHILDREN);

  pid_t pids[NUM_CHILDREN];

  for (int i = 0; i < NUM_CHILDREN; i++) {
    pid_t pid = fork();

    if (pid < 0) {
      handle_error(ERR_FORK_FAILED);
      return;
    } else if (pid == 0) {
      // Child
      usleep(100000); // Brief pause to order output
      child_task(i + 1);
      exit(0);
    } else {
      // Parent
      pids[i] = pid;
      printf("  [PARENT] Forked child #%d (PID: %d)\n", i + 1, pid);
    }
  }

  // Parent waits for all children
  printf("\n  [PARENT] Waiting for all children to finish...\n\n");

  int status;
  for (int i = 0; i < NUM_CHILDREN; i++) {
    pid_t child_pid = wait(&status);
    if (WIFEXITED(status)) {
      printf("  [PARENT] Child (PID %d) exited with code: %d\n", child_pid,
             WEXITSTATUS(status));
    }
  }

  printf("\n  - All %d children have terminated.\n\n", NUM_CHILDREN);
}

void run_memory_isolation(void) {
  int shared_var = 100;

  printf("\n=== 3. Memory Isolation Demo ===\n");
  printf("  Parent PID: %d\n", getpid());
  printf("  Variable 'shared_var' = %d (before fork)\n\n", shared_var);

  pid_t pid = fork();

  if (pid < 0) {
    handle_error(ERR_FORK_FAILED);
    return;
  } else if (pid == 0) {
    // Child modifies its own copy
    shared_var += 500;
    printf("  [CHILD]  PID: %d\n", getpid());
    printf("  [CHILD]  Modified shared_var += 500\n");
    printf("  [CHILD]  shared_var = %d (in child's memory space)\n",
           shared_var);
    exit(0);
  } else {
    // Parent waits, then checks its own copy
    int status;
    waitpid(pid, &status, 0);

    printf("  [PARENT] PID: %d\n", getpid());
    printf("  [PARENT] shared_var = %d (unchanged in parent's memory)\n\n",
           shared_var);
    printf("  Conclusion:\n");
    printf("  - Each process has its own memory space (copy-on-write).\n");
    printf("  - Changes in child do NOT affect parent variables.\n\n");
  }
}

void child_task(int id) {
  printf("  [CHILD #%d] PID: %d | Computing sum 1..1000...\n", id, getpid());

  long sum = 0;
  for (int j = 1; j <= 1000; j++) {
    sum += j;
  }

  usleep(rand() % 200000);
  printf("  [CHILD #%d] PID: %d | Result: %ld\n", id, getpid(), sum);
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
