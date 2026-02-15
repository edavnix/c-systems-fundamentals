/*
 ===============================================================================
 Exercise: 01_search_comparator.c
 Description: Performance comparison between Linear and Binary Search
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Dynamic Array Generation (Sorted)
 - Algorithm 1: Linear Search O(n)
 - Algorithm 2: Binary Search O(log n)
 - Performance Metrics: Time (seconds) and Comparison Count
 - Automatic efficiency calculation and recommendation
 - Dynamic memory management with proper cleanup
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 2

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_MEMORY_ALLOCATION
} Status;

typedef struct {
  int index;
  long comparisons;
  double time_taken;
} SearchResult;

void show_menu(void);
void handle_error(Status status);
void run_comparison_mode(void);
void run_algorithm_explanation(void);

void clear_input_buffer(void);
Status read_integer(int *value);

Status generate_sorted_array(int **arr, int size);
SearchResult linear_search(const int *arr, int size, int target);
SearchResult binary_search(const int *arr, int size, int target);
void print_array_preview(const int *arr, int size);

int main(void) {
  int option = 0;
  srand(time(NULL));

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION + 1) { // Hidden option for exit or just Next
      printf("\nExiting. Goodbye!\n");
      break;
    }

    // Standard Exit Option logic if preferred, but simpler 1-2 menu structure:
    if (option == 3) {
      printf("\nExiting. Goodbye!\n");
      break;
    }

    if (option < MIN_OPTION || option > 3) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    switch (option) {
    case 1:
      run_comparison_mode();
      break;
    case 2:
      run_algorithm_explanation();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Search Algorithm Comparator ===\n\n");
  printf("1. Run Performance Comparison\n");
  printf("2. Algorithm Explanations\n");
  printf("3. Exit\n");
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

void run_comparison_mode(void) {
  int size = 0;
  int *arr = NULL;
  int target;

  printf("\nEnter array size (e.g., 100, 10000, 1000000): ");
  if (read_integer(&size) != SUCCESS || size <= 0) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("Generating sorted array of %d elements...\n", size);
  if (generate_sorted_array(&arr, size) != SUCCESS) {
    handle_error(ERR_MEMORY_ALLOCATION);
    return;
  }

  print_array_preview(arr, size);

  printf("\nElement to search: ");
  if (read_integer(&target) != SUCCESS) {
    free(arr);
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("\n=== Linear Search ===\n");
  SearchResult lin_res = linear_search(arr, size, target);
  if (lin_res.index != -1) {
    printf("  - Found at index: %d\n", lin_res.index);
  } else {
    printf("  - Status: Not Found\n");
  }
  printf("  - Comparisons: %ld\n", lin_res.comparisons);
  printf("  - Time: %.6f sec\n", lin_res.time_taken);
  printf("  - Complexity: O(n)\n");

  printf("\n=== Binary Search ===\n");
  SearchResult bin_res = binary_search(arr, size, target);
  if (bin_res.index != -1) {
    printf("  - Found at index: %d\n", bin_res.index);
  } else {
    printf("  - Status: Not Found\n");
  }
  printf("  - Comparisons: %ld\n", bin_res.comparisons);
  printf("  - Time: %.6f sec\n", bin_res.time_taken);
  printf("  - Complexity: O(log n)\n");

  printf("\n=== Conclusion ===\n");
  if (bin_res.comparisons > 0 && lin_res.comparisons > 0) {
    double reduction = 100.0 *
                       (double)(lin_res.comparisons - bin_res.comparisons) /
                       lin_res.comparisons;
    printf("  - Optimization: Binary Search used %.1f%% fewer comparisons.\n",
           reduction);
  }
  printf("  - Recommendation: %s\n\n",
         size < 20 ? "Linear Search (Simplicity)" : "Binary Search (Speed)");

  free(arr);
}

void run_algorithm_explanation(void) {
  printf("\n=== Algorithm Logic ===\n\n");
  printf("1. Linear Search (O(n)):\n");
  printf("   - Iterates through every element from start to finish.\n");
  printf("   - Simple but slow for large datasets.\n\n");
  printf("2. Binary Search (O(log n)):\n");
  printf("   - Requires a SORTED array.\n");
  printf("   - Repeatedly divides the search interval in half.\n");
  printf("   - Extremely fast for large datasets.\n\n");
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

Status generate_sorted_array(int **arr, int size) {
  *arr = (int *)malloc(size * sizeof(int));
  if (*arr == NULL) {
    return ERR_MEMORY_ALLOCATION;
  }

  // Generate sorted data: Each element is greater than the previous
  (*arr)[0] = rand() % 10;
  for (int i = 1; i < size; i++) {
    (*arr)[i] = (*arr)[i - 1] + (rand() % 3 + 1);
  }

  return SUCCESS;
}

SearchResult linear_search(const int *arr, int size, int target) {
  SearchResult res = {-1, 0, 0.0};
  clock_t start = clock();

  for (int i = 0; i < size; i++) {
    res.comparisons++;
    if (arr[i] == target) {
      res.index = i;
      break;
    }
    // Optimization for sorted array: stop early if current > target
    if (arr[i] > target) {
      break;
    }
  }

  clock_t end = clock();
  res.time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

  return res;
}

SearchResult binary_search(const int *arr, int size, int target) {
  SearchResult res = {-1, 0, 0.0};
  clock_t start = clock();

  int low = 0;
  int high = size - 1;

  while (low <= high) {
    res.comparisons++;
    int mid = low + (high - low) / 2;

    if (arr[mid] == target) {
      res.index = mid;
      break;
    }

    if (arr[mid] < target) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }

  clock_t end = clock();
  res.time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

  return res;
}

void print_array_preview(const int *arr, int size) {
  printf("Data Preview: [");
  if (size <= 10) {
    for (int i = 0; i < size; i++) {
      printf("%d%s", arr[i], (i < size - 1) ? ", " : "");
    }
  } else {
    for (int i = 0; i < 5; i++) {
      printf("%d, ", arr[i]);
    }
    printf("..., ");
    printf("%d, %d", arr[size - 2], arr[size - 1]);
  }
  printf("]\n");
}
