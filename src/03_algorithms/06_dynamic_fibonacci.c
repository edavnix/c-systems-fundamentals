/*
 ===============================================================================
 Exercise: 06_dynamic_fibonacci.c
 Description: Dynamic Programming demonstration using Fibonacci
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Pure Recursion O(2ⁿ) (with safety cutoff)
 - Top-Down Dynamic Programming (Memoization) O(n)
 - Bottom-Up Dynamic Programming (Iterative) O(n) time, O(1) space
 - Performance tracking (Time, subproblems, iterations)
 - Estimation logic for pure recursion comparison
 - Interactive menu for repeated benchmarks
 ===============================================================================
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define RECURSION_LIMIT 45
#define MIN_OPTION 1
#define MAX_OPTION 3

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_MEMORY_ALLOCATION
} Status;

typedef struct {
  unsigned long long result;
  double time_taken;
  int calculated;
  int reused;
  int iterations;
} DPStats;

void show_menu(void);
void handle_error(Status status);
void run_benchmark(void);
void run_algorithm_info(void);

void clear_input_buffer(void);
Status read_integer(int *value);

unsigned long long fib_pure(int n);
unsigned long long fib_memo(int n, unsigned long long *memo, int *calculated,
                            int *reused);
unsigned long long fib_iterative(int n, int *iterations);
void show_comparison(int n, double est_pure_time, DPStats memo_stats,
                     DPStats iter_stats);
void show_memo_table(const unsigned long long *memo, int n);

int main(void) {
  int option = 0;
  srand(time(NULL));

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
      run_benchmark();
      break;
    case 2:
      run_algorithm_info();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Fibonacci Dynamic Programming ===\n\n");
  printf("1. Run Performance Benchmark\n");
  printf("2. Algorithm Information\n");
  printf("3. Exit\n");
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
  case ERR_MEMORY_ALLOCATION:
    printf("Error: Memory allocation failed.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_benchmark(void) {
  int n = 0;
  printf("\nEnter Fibonacci term to calculate (e.g., 40, 50): ");

  if (read_integer(&n) != SUCCESS || n < 0) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("\n--- Performance Benchmark for n = %d ---\n", n);

  // Allocate memo
  unsigned long long *memo =
      (unsigned long long *)calloc(n + 1, sizeof(unsigned long long));
  if (memo == NULL) {
    handle_error(ERR_MEMORY_ALLOCATION);
    return;
  }

  DPStats memo_stats = {0, 0.0, 0, 0, 0};
  DPStats iter_stats = {0, 0.0, 0, 0, 0};
  double est_pure_time = 0.0;

  // 1. Pure Recursion
  printf("\n[1] Pure Recursion:\n");
  if (n > RECURSION_LIMIT) {
    // Estimate: O(1.618^n). 0.01ns per op is a loose baseline.
    est_pure_time = 0.00000001 * pow(1.618034, n);
    printf("  - Result: SKIPPED (Cutoff: n > %d)\n", RECURSION_LIMIT);
    printf("  - Est. Time: ~%.2f seconds\n", est_pure_time);
  } else {
    clock_t start = clock();
    unsigned long long res = fib_pure(n);
    clock_t end = clock();
    est_pure_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("  - Result: %llu\n", res);
    printf("  - Time:   %.6f seconds\n", est_pure_time);
  }

  // 2. Memoization
  printf("\n[2] Top-Down (Memoization):\n");
  clock_t m_start = clock();
  memo_stats.result =
      fib_memo(n, memo, &memo_stats.calculated, &memo_stats.reused);
  clock_t m_end = clock();
  memo_stats.time_taken = ((double)(m_end - m_start)) / CLOCKS_PER_SEC;
  if (memo_stats.time_taken <= 0.0)
    memo_stats.time_taken = 0.000001;

  printf("  - Result: %llu\n", memo_stats.result);
  printf("  - Time:   %.6f seconds\n", memo_stats.time_taken);
  printf("  - Stats:  Calculated: %d, Reused: %d\n", memo_stats.calculated,
         memo_stats.reused);

  // 3. Iterative
  printf("\n[3] Bottom-Up (Iterative):\n");
  clock_t i_start = clock();
  iter_stats.result = fib_iterative(n, &iter_stats.iterations);
  clock_t i_end = clock();
  iter_stats.time_taken = ((double)(i_end - i_start)) / CLOCKS_PER_SEC;
  if (iter_stats.time_taken <= 0.0)
    iter_stats.time_taken = 0.000001;

  printf("  - Result: %llu\n", iter_stats.result);
  printf("  - Time:   %.6f seconds\n", iter_stats.time_taken);
  printf("  - Stats:  Iterations: %d\n", iter_stats.iterations);

  show_comparison(n, est_pure_time, memo_stats, iter_stats);
  show_memo_table(memo, n);

  free(memo);
  printf("\n");
}

void run_algorithm_info(void) {
  printf("\n=== Algorithm Information ===\n\n");
  printf("1. Pure Recursion:\n");
  printf("   - Approach: Divide and Conquer.\n");
  printf("   - Complexity: O(2ⁿ).\n");
  printf("   - Issue: Redundant calculations grow exponentially.\n\n");
  printf("2. Memoization (Top-Down):\n");
  printf("   - Approach: Store results of subproblems to avoid re-work.\n");
  printf("   - Complexity: O(n).\n");
  printf("   - Note: Recursive approach but with efficient memory use.\n\n");
  printf("3. Iterative (Bottom-Up):\n");
  printf("   - Approach: Build result from smallest cases upwards.\n");
  printf("   - Complexity: O(n) Time, O(1) Space.\n");
  printf("   - Note: Most efficient implementation for linear DP.\n\n");
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

unsigned long long fib_pure(int n) {
  if (n <= 1)
    return n;
  return fib_pure(n - 1) + fib_pure(n - 2);
}

unsigned long long fib_memo(int n, unsigned long long *memo, int *calculated,
                            int *reused) {
  if (n <= 1) {
    memo[n] = n;
    return n;
  }

  if (memo[n] != 0) {
    (*reused)++;
    return memo[n];
  }

  (*calculated)++;
  memo[n] = fib_memo(n - 1, memo, calculated, reused) +
            fib_memo(n - 2, memo, calculated, reused);

  return memo[n];
}

unsigned long long fib_iterative(int n, int *iterations) {
  if (n <= 1)
    return n;

  unsigned long long a = 0;
  unsigned long long b = 1;
  unsigned long long c = 0;

  for (int i = 2; i <= n; i++) {
    (*iterations)++;
    c = a + b;
    a = b;
    b = c;
  }

  return b;
}

void show_comparison(int n, double est_pure_time, DPStats memo_stats,
                     DPStats iter_stats) {
  printf("\n=== Optimization Analysis ===\n\n");
  double memo_gain = est_pure_time / memo_stats.time_taken;
  double iter_gain = est_pure_time / iter_stats.time_taken;

  printf("  - Memoization is ~%.0fx faster than Recursion.\n", memo_gain);
  printf("  - Iterative   is ~%.0fx faster than Recursion.\n", iter_gain);
  printf("  - Iterative   is ~%.0fx faster than Memoization.\n",
         memo_stats.time_taken / iter_stats.time_taken);
}

void show_memo_table(const unsigned long long *memo, int n) {
  printf("\nMemoization Table Preview (1st 10):\n  ");
  int limit = (n < 9) ? n + 1 : 10;
  for (int i = 0; i < limit; i++) {
    printf("[%d]:%llu ", i, memo[i]);
  }
  printf("\n");
}
