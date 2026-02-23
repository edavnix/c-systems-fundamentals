/*
 ===============================================================================
 Exercise: 09_string_matching.c
 Description: String Matching Algorithms (Brute Force vs KMP)
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Brute Force substring search O(n*m)
 - KMP (Knuth-Morris-Pratt) optimal search O(n+m)
 - LPS (Longest Proper Prefix which is also Suffix) array construction
 - Comparison counting and execution time tracking
 - Interactive menu for repeated searches
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TEXT 1000
#define MAX_PATTERN 100
#define MIN_OPTION 1
#define MAX_OPTION 4
#define TRUE 1
#define FALSE 0

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_MEMORY_ALLOCATION
} Status;

typedef struct {
  int found_index;
  int comparisons;
  double time_taken;
} MatchStats;

void show_menu(void);
void handle_error(Status status);
void run_demo_search(void);
void run_custom_search(void);
void run_algorithm_info(void);

void clear_input_buffer(void);
Status read_integer(int *value);
Status read_string(char *buffer, int max_len);

void run_brute_force(const char *text, const char *pattern, MatchStats *stats);
void run_kmp(const char *text, const char *pattern, MatchStats *stats);
void compute_lps_array(const char *pattern, int M, int *lps);
void print_lps_array(int *lps, int M);
void show_comparison(MatchStats bf_stats, MatchStats kmp_stats);

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
      run_demo_search();
      break;
    case 2:
      run_custom_search();
      break;
    case 3:
      run_algorithm_info();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== String Matching Algorithms ===\n\n");
  printf("1. Run Demo (Brute Force vs KMP)\n");
  printf("2. Run Custom Search\n");
  printf("3. Algorithm Information\n");
  printf("4. Exit\n");
  printf("Option: ");
}

void handle_error(Status status) {
  switch (status) {
  case ERR_INVALID_INPUT:
    printf("Error: Invalid input. Please enter a valid value.\n\n");
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

void run_demo_search(void) {
  char text[] = "ABABDABACDABABCABAB";
  char pattern[] = "ABABCABAB";

  printf("\n=== Demo Case ===\n");
  printf("Text:    \"%s\"\n", text);
  printf("Pattern: \"%s\"\n", pattern);

  MatchStats bf_stats = {-1, 0, 0.0};
  MatchStats kmp_stats = {-1, 0, 0.0};

  run_brute_force(text, pattern, &bf_stats);
  run_kmp(text, pattern, &kmp_stats);

  show_comparison(bf_stats, kmp_stats);
}

void run_custom_search(void) {
  char text[MAX_TEXT];
  char pattern[MAX_PATTERN];

  printf("\nEnter text to search in:\n  > ");
  if (read_string(text, MAX_TEXT) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("Enter pattern to find:\n  > ");
  if (read_string(pattern, MAX_PATTERN) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("\n=== Custom Search ===\n");
  printf("Text:    \"%s\"\n", text);
  printf("Pattern: \"%s\"\n", pattern);

  MatchStats bf_stats = {-1, 0, 0.0};
  MatchStats kmp_stats = {-1, 0, 0.0};

  run_brute_force(text, pattern, &bf_stats);
  run_kmp(text, pattern, &kmp_stats);

  show_comparison(bf_stats, kmp_stats);
}

void run_algorithm_info(void) {
  printf("\n=== Algorithm Information ===\n\n");
  printf("1. Brute Force O(n*m):\n");
  printf("   - Compares pattern at every position in text.\n");
  printf("   - Simple but slow for large inputs.\n\n");
  printf("2. KMP O(n+m):\n");
  printf("   - Precomputes LPS (Longest Prefix Suffix) table.\n");
  printf("   - Avoids redundant comparisons using earlier match info.\n");
  printf("   - Optimal for long texts with repetitive patterns.\n\n");
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

Status read_string(char *buffer, int max_len) {
  if (fgets(buffer, max_len, stdin) == NULL) {
    return ERR_INVALID_INPUT;
  }
  size_t len = strlen(buffer);
  if (len > 0 && buffer[len - 1] == '\n') {
    buffer[len - 1] = '\0';
  }
  return SUCCESS;
}

void run_brute_force(const char *text, const char *pattern, MatchStats *stats) {
  printf("\n[1] Brute Force:\n");

  int N = strlen(text);
  int M = strlen(pattern);

  clock_t start = clock();

  for (int i = 0; i <= N - M; i++) {
    int j;
    for (j = 0; j < M; j++) {
      stats->comparisons++;
      if (text[i + j] != pattern[j]) {
        break;
      }
    }
    if (j == M) {
      stats->found_index = i;
      break;
    }
  }

  clock_t end = clock();
  stats->time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
  if (stats->time_taken <= 0.0)
    stats->time_taken = 0.000001;

  if (stats->found_index != -1) {
    printf("  - Found at index: %d\n", stats->found_index);
  } else {
    printf("  - Status: Not Found\n");
  }
  printf("  - Comparisons: %d\n", stats->comparisons);
  printf("  - Time:        %.6f seconds\n", stats->time_taken);
  printf("  - Complexity:  O(n*m)\n");
}

void run_kmp(const char *text, const char *pattern, MatchStats *stats) {
  printf("\n[2] KMP (Knuth-Morris-Pratt):\n");

  int N = strlen(text);
  int M = strlen(pattern);

  int *lps = (int *)malloc(M * sizeof(int));
  if (!lps) {
    handle_error(ERR_MEMORY_ALLOCATION);
    return;
  }

  compute_lps_array(pattern, M, lps);
  print_lps_array(lps, M);

  clock_t start = clock();

  int i = 0;
  int j = 0;

  while (i < N) {
    stats->comparisons++;
    if (pattern[j] == text[i]) {
      j++;
      i++;
    }

    if (j == M) {
      stats->found_index = i - j;
      break;
    } else if (i < N && pattern[j] != text[i]) {
      if (j != 0) {
        j = lps[j - 1];
      } else {
        i++;
      }
    }
  }

  clock_t end = clock();
  stats->time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
  if (stats->time_taken <= 0.0)
    stats->time_taken = 0.000001;

  free(lps);

  if (stats->found_index != -1) {
    printf("  - Found at index: %d\n", stats->found_index);
  } else {
    printf("  - Status: Not Found\n");
  }
  printf("  - Comparisons: %d\n", stats->comparisons);
  printf("  - Time:        %.6f seconds\n", stats->time_taken);
  printf("  - Complexity:  O(n+m)\n");
}

void compute_lps_array(const char *pattern, int M, int *lps) {
  int len = 0;
  lps[0] = 0;
  int i = 1;

  while (i < M) {
    if (pattern[i] == pattern[len]) {
      len++;
      lps[i] = len;
      i++;
    } else {
      if (len != 0) {
        len = lps[len - 1];
      } else {
        lps[i] = 0;
        i++;
      }
    }
  }
}

void print_lps_array(int *lps, int M) {
  printf("  - LPS Table: [");
  for (int i = 0; i < M; i++) {
    printf("%d%s", lps[i], (i < M - 1) ? ", " : "");
  }
  printf("]\n");
}

void show_comparison(MatchStats bf_stats, MatchStats kmp_stats) {
  printf("\n=== Comparison ===\n");

  if (bf_stats.comparisons > 0 && kmp_stats.comparisons > 0) {
    int diff = bf_stats.comparisons - kmp_stats.comparisons;
    double efficiency = ((double)diff / bf_stats.comparisons) * 100.0;

    if (efficiency > 0) {
      printf("  - KMP used %.0f%% fewer comparisons than Brute Force.\n",
             efficiency);
    } else {
      printf("  - Both algorithms had similar performance for this case.\n");
    }
  }

  printf("  - Recommendation: KMP is optimal for large texts with repetitive "
         "patterns.\n\n");
}
