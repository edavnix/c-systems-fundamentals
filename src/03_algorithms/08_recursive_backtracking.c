/*
 ===============================================================================
 Exercise: 08_recursive_backtracking.c
 Description: N-Queens Problem using Backtracking
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Backtracking algorithm implementation
 - Pruning of invalid branches (is_safe check)
 - Visual representation of solutions
 - Performance benchmarking (Time & Scale)
 - Interactive menu for different problem sizes
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define MAX_N 20
#define MIN_OPTION 1
#define MAX_OPTION 4

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_BOARD_TOO_LARGE,
  ERR_BOARD_TOO_SMALL
} Status;

void show_menu(void);
void handle_error(Status status);
void run_demo_solve(void);
void run_custom_solve(void);
void run_algorithm_info(void);

void clear_input_buffer(void);
Status read_integer(int *value);

void solve_n_queens(int row, int n, int board[], int *count, int print_boards);
int is_safe(int board[], int row, int col);
void print_board(int board[], int n, int sol_num);

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
      run_demo_solve();
      break;
    case 2:
      run_custom_solve();
      break;
    case 3:
      run_algorithm_info();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== N-Queens Backtracking Solver ===\n\n");
  printf("1. Run Demo Case (N=4)\n");
  printf("2. Run Custom Case (Manual N)\n");
  printf("3. Algorithm Information\n");
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
  case ERR_BOARD_TOO_LARGE:
    printf("Error: Board size too large (Max is %d).\n\n", MAX_N);
    break;
  case ERR_BOARD_TOO_SMALL:
    printf("Error: Board size must be greater than 0.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_demo_solve(void) {
  int n = 4;
  int board[MAX_N];
  int count = 0;

  printf("\n--- Running Demo (N=4) ---\n");
  printf("Searching for solutions...\n\n");

  clock_t start = clock();
  solve_n_queens(0, n, board, &count, TRUE);
  clock_t end = clock();

  double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
  if (time_taken <= 0.0)
    time_taken = 0.000001;

  printf("  - Total solutions found: %d\n", count);
  printf("  - Execution time:        %.6f seconds\n\n", time_taken);
}

void run_custom_solve(void) {
  int n = 0;
  int board[MAX_N];
  int count = 0;

  printf("\nEnter board size (N): ");
  if (read_integer(&n) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  if (n <= 0) {
    handle_error(ERR_BOARD_TOO_SMALL);
    return;
  }

  if (n > MAX_N) {
    handle_error(ERR_BOARD_TOO_LARGE);
    return;
  }

  printf("\n--- Solving N=%d ---\n", n);
  printf("Searching for solutions...\n\n");

  int print_limit = (n > 10) ? FALSE : TRUE;
  if (!print_limit) {
    printf("  (Solutions visualization disabled for N > 10 for performance)\n");
  }

  clock_t start = clock();
  solve_n_queens(0, n, board, &count, print_limit);
  clock_t end = clock();

  double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
  if (time_taken <= 0.0)
    time_taken = 0.000001;

  printf("  - Total solutions found: %d\n", count);
  printf("  - Execution time:        %.6f seconds\n\n", time_taken);
}

void run_algorithm_info(void) {
  printf("\n=== Algorithm Information ===\n\n");
  printf("Backtracking (N-Queens):\n");
  printf("  - Strategy: Decision Tree with Pruning.\n");
  printf("  - Search:   Depth-First Search (DFS).\n");
  printf("  - Pruning:  Whenever a queen is placed, future rows are "
         "restricted.\n");
  printf("  - Complexity: Exponential, but significantly faster than Brute "
         "Force.\n");
  printf("  - Applications: Pathfinding, Sudoku, Cryptarithmetic.\n\n");
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

void solve_n_queens(int row, int n, int board[], int *count, int print_boards) {
  if (row == n) {
    (*count)++;
    if (print_boards) {
      print_board(board, n, *count);
    }
    return;
  }

  for (int col = 0; col < n; col++) {
    if (is_safe(board, row, col)) {
      board[row] = col;
      solve_n_queens(row + 1, n, board, count, print_boards);
    }
  }
}

int is_safe(int board[], int row, int col) {
  for (int i = 0; i < row; i++) {
    // Check column and diagonals
    if (board[i] == col || abs(board[i] - col) == abs(i - row)) {
      return FALSE;
    }
  }
  return TRUE;
}

void print_board(int board[], int n, int sol_num) {
  printf("Solution %d:\n", sol_num);
  for (int r = 0; r < n; r++) {
    printf("  ");
    for (int c = 0; c < n; c++) {
      if (board[r] == c) {
        printf("Q ");
      } else {
        printf(". ");
      }
    }
    printf("\n");
  }
  printf("\n");
}
