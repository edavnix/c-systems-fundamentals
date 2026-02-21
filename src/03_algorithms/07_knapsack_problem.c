/*
 ===============================================================================
 Exercise: 07_knapsack_problem.c
 Description: 0/1 Knapsack Problem using Dynamic Programming
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Dynamic Programming implementation O(n*W)
 - DP Table visualization (Partial)
 - Backtracking to find selected items
 - Custom item input and Demo mode
 - Memory management with 2D dynamic arrays
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define MAX_ITEMS 100
#define MIN_OPTION 1
#define MAX_OPTION 3

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_MEMORY_ALLOCATION
} Status;

typedef struct {
  int id;
  int weight;
  int value;
  double ratio;
} Item;

void show_menu(void);
void handle_error(Status status);
void run_demo_knapsack(void);
void run_custom_knapsack(void);

void clear_input_buffer(void);
Status read_integer(int *value);

void solve_knapsack(int W, Item *items, int n);
int max_val(int a, int b);
void print_items(Item *items, int n);
void print_dp_table_partial(int **dp, int n, int W);

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
      run_demo_knapsack();
      break;
    case 2:
      run_custom_knapsack();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== 0/1 Knapsack DP Visualizer ===\n\n");
  printf("1. Run Demo Case (W=50, 4 items)\n2. Run Custom Case\n3. Exit\n");
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

void run_demo_knapsack(void) {
  int W = 50;
  int n = 4;
  Item items[MAX_ITEMS];

  int w_vals[] = {10, 20, 30, 15};
  int v_vals[] = {60, 100, 120, 80};

  for (int i = 0; i < 4; i++) {
    items[i].id = i + 1;
    items[i].weight = w_vals[i];
    items[i].value = v_vals[i];
    items[i].ratio = (double)v_vals[i] / w_vals[i];
  }

  printf("\n=== Demo Configuration ===\n");
  printf("Capacity: %d kg\n", W);
  print_items(items, n);
  solve_knapsack(W, items, n);
}

void run_custom_knapsack(void) {
  int W, n;
  Item items[MAX_ITEMS];

  printf("\nKnapsack Capacity: ");
  if (read_integer(&W) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("Number of items: ");
  if (read_integer(&n) != SUCCESS || n <= 0 || n > MAX_ITEMS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  for (int i = 0; i < n; i++) {
    items[i].id = i + 1;
    printf("Item %d - Weight: ", items[i].id);
    if (read_integer(&items[i].weight) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      return;
    }
    printf("Item %d - Value:  ", items[i].id);
    if (read_integer(&items[i].value) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      return;
    }
    items[i].ratio = (double)items[i].value / items[i].weight;
  }

  printf("\n=== Custom Configuration ===\n");
  printf("Capacity: %d kg\n", W);
  print_items(items, n);
  solve_knapsack(W, items, n);
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

int max_val(int a, int b) { return (a > b) ? a : b; }

void solve_knapsack(int W, Item *items, int n) {
  printf("\n  - Building DP Table (%dx%d)...\n", n + 1, W + 1);

  int **dp = (int **)malloc((n + 1) * sizeof(int *));
  if (dp == NULL) {
    handle_error(ERR_MEMORY_ALLOCATION);
    return;
  }
  for (int i = 0; i <= n; i++) {
    dp[i] = (int *)malloc((W + 1) * sizeof(int));
    if (dp[i] == NULL) {
      for (int k = 0; k < i; k++)
        free(dp[k]);
      free(dp);
      handle_error(ERR_MEMORY_ALLOCATION);
      return;
    }
  }

  for (int i = 0; i <= n; i++) {
    for (int w = 0; w <= W; w++) {
      if (i == 0 || w == 0) {
        dp[i][w] = 0;
      } else if (items[i - 1].weight <= w) {
        dp[i][w] =
            max_val(items[i - 1].value + dp[i - 1][w - items[i - 1].weight],
                    dp[i - 1][w]);
      } else {
        dp[i][w] = dp[i - 1][w];
      }
    }
  }

  print_dp_table_partial(dp, n, W);

  int res = dp[n][W];
  int w = W;
  int total_weight = 0;
  Item selected[MAX_ITEMS];
  int selected_count = 0;

  for (int i = n; i > 0 && res > 0; i--) {
    if (res != dp[i - 1][w]) {
      selected[selected_count++] = items[i - 1];
      total_weight += items[i - 1].weight;
      res -= items[i - 1].value;
      w -= items[i - 1].weight;
    }
  }

  printf("\nOptimal Solution:\n");
  printf("  - Max Value:  $%d\n", dp[n][W]);
  printf("  - Selected Items:\n");
  for (int i = selected_count - 1; i >= 0; i--) {
    printf("    [%d] ID:%d (W:%d, V:$%d)\n", (selected_count - i),
           selected[i].id, selected[i].weight, selected[i].value);
  }

  printf("  - Total Weight: %dkg / %dkg\n", total_weight, W);
  printf("  - Complexity:   O(n*W)\n\n");

  for (int i = 0; i <= n; i++) {
    free(dp[i]);
  }
  free(dp);
}

void print_items(Item *items, int n) {
  printf("Available Items:\n");
  printf("  ID | Weight | Value | Ratio\n");
  printf("  ---------------------------\n");
  for (int i = 0; i < n; i++) {
    printf("  %2d | %4dkg | $%4d | %.2f\n", items[i].id, items[i].weight,
           items[i].value, items[i].ratio);
  }
}

void print_dp_table_partial(int **dp, int n, int W) {
  printf("  - DP Table Preview:\n\n");
  int cols[6];
  for (int i = 0; i < 6; i++) {
    cols[i] = (W * i) / 5;
  }

  printf("        ");
  for (int j = 0; j < 6; j++) {
    printf("%4dkg ", cols[j]);
  }
  printf("\n");

  for (int i = 0; i <= n; i++) {
    printf("  obj%-2d ", i);
    for (int j = 0; j < 6; j++) {
      printf("%6d ", dp[i][cols[j]]);
    }
    printf("\n");
  }
}
