/*
 ===============================================================================
 Exercise: 10_algorithm_complexity.c
 Description: Algorithmic Complexity Analyzer (Big O)
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Operation counting and timing for multiple algorithms
 - Bubble Sort O(n²) vs Binary Search O(log n) demonstration
 - ASCII bar chart for visual complexity representation
 - Configurable array sizes for benchmarking
 - Interactive menu for repeated analysis
 ===============================================================================
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 4

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_MEMORY_ALLOCATION
} Status;

typedef struct {
  int size;
  double time_ms;
  long long operations;
} TestResult;

void show_menu(void);
void handle_error(Status status);
void run_bubble_sort_analysis(void);
void run_binary_search_analysis(void);
void run_complexity_info(void);

void clear_input_buffer(void);
Status read_integer(int *value);

long long algo_bubble_sort(int *arr, int n);
long long algo_binary_search(int *arr, int n, int target);
void print_graph(TestResult *results, int count, const char *label);
void print_operations(long long ops);

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
      run_bubble_sort_analysis();
      break;
    case 2:
      run_binary_search_analysis();
      break;
    case 3:
      run_complexity_info();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Algorithm Complexity Analyzer ===\n\n");
  printf("1. Analyze Bubble Sort   O(n²)\n");
  printf("2. Analyze Binary Search O(log n)\n");
  printf("3. Big O Complexity Info\n");
  printf("4. Exit\n");
  printf("Option: ");
}

void handle_error(Status status) {
  switch (status) {
  case ERR_INVALID_INPUT:
    printf("Error: Invalid input. Please enter a number.\n\n");
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

void run_bubble_sort_analysis(void) {
  int sizes[] = {100, 200, 400, 800, 1600};
  int num_tests = sizeof(sizes) / sizeof(sizes[0]);
  TestResult results[5];

  printf("\n=== Bubble Sort O(n²) Analysis ===\n\n");
  printf("%-6s | %-12s | %s\n", "Size", "Time (ms)", "Operations");
  printf("-------|--------------|------------\n");

  for (int i = 0; i < num_tests; i++) {
    int n = sizes[i];
    int *arr = (int *)malloc(n * sizeof(int));
    if (arr == NULL) {
      handle_error(ERR_MEMORY_ALLOCATION);
      return;
    }

    // Worst case: reverse sorted array
    for (int j = 0; j < n; j++) {
      arr[j] = n - j;
    }

    clock_t start = clock();
    long long ops = algo_bubble_sort(arr, n);
    clock_t end = clock();

    double time_ms = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    if (time_ms <= 0.0)
      time_ms = 0.001;

    results[i].size = n;
    results[i].time_ms = time_ms;
    results[i].operations = ops;

    printf("%-6d | %9.3f    | ", n, time_ms);
    print_operations(ops);
    printf("\n");

    free(arr);
  }

  print_graph(results, num_tests, "n²");

  printf("\n  - Complexity Detected: O(n²)\n");
  printf("  - Growth Rate: Doubling n → 4x more operations.\n\n");
}

void run_binary_search_analysis(void) {
  int sizes[] = {100, 1000, 10000, 100000, 1000000};
  int num_tests = sizeof(sizes) / sizeof(sizes[0]);
  TestResult results[5];

  printf("\n=== Binary Search O(log n) Analysis ===\n\n");
  printf("%-8s | %-12s | %s\n", "Size", "Time (ms)", "Operations");
  printf("---------|--------------|------------\n");

  for (int i = 0; i < num_tests; i++) {
    int n = sizes[i];
    int *arr = (int *)malloc(n * sizeof(int));
    if (arr == NULL) {
      handle_error(ERR_MEMORY_ALLOCATION);
      return;
    }

    // Sorted array (pre-condition for binary search)
    for (int j = 0; j < n; j++) {
      arr[j] = j * 2;
    }

    int target = arr[n - 1]; // Worst case: last element

    clock_t start = clock();
    long long ops = algo_binary_search(arr, n, target);
    clock_t end = clock();

    double time_ms = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    if (time_ms <= 0.0)
      time_ms = 0.001;

    results[i].size = n;
    results[i].time_ms = time_ms;
    results[i].operations = ops;

    printf("%-8d | %9.4f    | %lld\n", n, time_ms, ops);

    free(arr);
  }

  printf("\n  - Complexity Detected: O(log n)\n");
  printf("  - Growth Rate: Multiplying n×10 → only ~3 more operations.\n\n");
}

void run_complexity_info(void) {
  printf("\n=== Big O Complexity Reference ===\n\n");
  printf("  Notation   | Name          | Example\n");
  printf("  -----------|---------------|----------------------\n");
  printf("  O(1)       | Constant      | Array access\n");
  printf("  O(log n)   | Logarithmic   | Binary Search\n");
  printf("  O(n)       | Linear        | Linear Search\n");
  printf("  O(n log n) | Log-Linear    | Merge Sort, Quick Sort\n");
  printf("  O(n²)      | Quadratic     | Bubble Sort\n");
  printf("  O(2ⁿ)      | Exponential   | N-Queens (brute force)\n\n");
  printf("  Rule: For n=1000, prefer O(n log n) or better.\n\n");
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

long long algo_bubble_sort(int *arr, int n) {
  long long operations = 0;
  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      operations++;
      if (arr[j] > arr[j + 1]) {
        int temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
      }
    }
  }
  return operations;
}

long long algo_binary_search(int *arr, int n, int target) {
  long long operations = 0;
  int low = 0, high = n - 1;
  while (low <= high) {
    operations++;
    int mid = low + (high - low) / 2;
    if (arr[mid] == target)
      break;
    else if (arr[mid] < target)
      low = mid + 1;
    else
      high = mid - 1;
  }
  return operations;
}

void print_graph(TestResult *results, int count, const char *label) {
  long long base = results[0].operations;
  int scale = (base > 0) ? (int)(base / 4) : 1;

  printf("\nGrowth Chart (%s):\n", label);
  for (int i = 0; i < count; i++) {
    printf("  n=%-5d | ", results[i].size);
    int blocks = (int)(results[i].operations / scale);
    if (blocks > 60) {
      printf("(off scale)\n");
    } else {
      for (int b = 0; b < blocks; b++) {
        printf("█");
      }
      printf("\n");
    }
  }
}

void print_operations(long long ops) {
  if (ops >= 1000000) {
    printf("%.2fM", (double)ops / 1000000.0);
  } else if (ops >= 1000) {
    printf("%.1fK", (double)ops / 1000.0);
  } else {
    printf("%lld", ops);
  }
}
