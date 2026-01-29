/*
 ===============================================================================
 Exercise: 04_recursive_iterative_operations.c
 Description: Performance comparison between recursive and iterative methods
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Factorial calculation (recursive vs iterative)
 - Fibonacci sequence generation (recursive vs iterative)
 - Sum of natural numbers (recursive vs iterative)
 - Power computation (recursive vs iterative)
 - Execution time measurement
 - Call/iteration counting
 - Performance recommendations based on results
 ===============================================================================
*/

#include <stdio.h>
#include <time.h>

#define TRUE 1
#define MIN_OPTION 1
#define MAX_OPTION 5
#define MIN_SPEED_DIFF 1.5
#define MIN_MEASURABLE_TIME 0.000001
#define MAX_FIBONACCI_TERM 20

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_NEGATIVE_VAL,
  ERR_FIBONACCI_LIMIT,
  ERR_FIBONACCI_INVALID,
  ERR_UNDEFINED
} Status;

typedef struct {
  Status status;
  double value;
  long count;
} Result;

void show_menu(void);
void handle_error(Status status);
void run_comparison(int option);

void clear_input_buffer(void);
Status read_integer(int *value);
Status read_double(double *value);

Result factorial_rec(int n);
Result factorial_ite(int n);
Result fibonacci_rec(int n);
Result fibonacci_ite(int n);
Result sum_natural_rec(int n);
Result sum_natural_ite(int n);
Result power_rec(double base, int exp);
Result power_ite(double base, int exp);

int main(void) {
  int option = 0;

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nThank you for using the comparison tool!\n");
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    run_comparison(option);
  }

  return 0;
}

void show_menu(void) {
  printf("===== Recursive vs Iterative Operations =====\n\n");
  printf("1. Factorial\n2. Fibonacci\n3. Sum of naturals\n4. Power\n5. Exit\n");
  printf("Select operation: ");
}

void handle_error(Status status) {
  switch (status) {
  case ERR_INVALID_INPUT:
    printf("Error: Invalid input.\n\n");
    break;
  case ERR_NEGATIVE_VAL:
    printf("Error: Operation not defined for negative values.\n\n");
    break;
  case ERR_FIBONACCI_INVALID:
    printf("Error: Fibonacci sequence starts at term 1.\n\n");
    break;
  case ERR_FIBONACCI_LIMIT:
    printf("Error: Exceeds the maximum (%d).\n\n", MAX_FIBONACCI_TERM);
    break;
  case ERR_UNDEFINED:
    printf("Error: Mathematical operation is undefined (0^0).\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_comparison(int option) {
  int n = 0, exp = 0;
  double base = 0;
  Result res_rec, res_ite;
  clock_t start, end;
  double time_rec, time_ite;

  if (option == 4) {
    printf("\nEnter base: ");
    if (read_double(&base) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      return;
    }

    printf("Enter exponent: ");
    if (read_integer(&exp) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      return;
    }

    n = exp;

  } else {
    if (option == 2) {
      printf("\nEnter term (n, max %d): ", MAX_FIBONACCI_TERM);
    } else {
      printf("\nEnter term (n): ");
    }
    if (read_integer(&n) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      return;
    }
  }

  start = clock();

  switch (option) {
  case 1:
    res_rec = factorial_rec(n);
    break;
  case 2:
    res_rec = fibonacci_rec(n);
    break;
  case 3:
    res_rec = sum_natural_rec(n);
    break;
  case 4:
    res_rec = power_rec(base, exp);
    break;
  default:
    return;
  }

  end = clock();
  time_rec = (double)(end - start) / CLOCKS_PER_SEC;

  if (res_rec.status != SUCCESS) {
    handle_error(res_rec.status);
    return;
  }

  start = clock();

  switch (option) {
  case 1:
    res_ite = factorial_ite(n);
    break;
  case 2:
    res_ite = fibonacci_ite(n);
    break;
  case 3:
    res_ite = sum_natural_ite(n);
    break;
  case 4:
    res_ite = power_ite(base, exp);
    break;
  default:
    return;
  }

  end = clock();
  time_ite = (double)(end - start) / CLOCKS_PER_SEC;

  printf("\n----- Methods -----\n\n");

  printf("Recursive:\n");
  printf("  - Result: %.2f\n", res_rec.value);
  printf("  - Time: %.8f seconds\n", time_rec);
  printf("  - Calls: %ld\n\n", res_rec.count);

  printf("Iterative:\n");
  printf("  - Result: %.2f\n", res_ite.value);
  printf("  - Time: %.8f seconds\n", time_ite);
  printf("  - Iterations: %ld\n\n", res_ite.count);

  printf("----- Comparison -----\n\n");
  if (time_rec < MIN_MEASURABLE_TIME && time_ite < MIN_MEASURABLE_TIME) {
    printf("  - Both methods executed too fast to measure accurately\n");
    printf("  - Recommendation: Either method is acceptable\n\n");
    return;
  }

  double speed_factor = 0.0;
  if (time_rec > time_ite && time_ite >= MIN_MEASURABLE_TIME) {
    speed_factor = time_rec / time_ite;
  } else if (time_ite > time_rec && time_rec >= MIN_MEASURABLE_TIME) {
    speed_factor = time_ite / time_rec;
  }

  if (speed_factor >= MIN_SPEED_DIFF) {
    printf("  - %s method was %.2fx faster\n",
           (time_ite < time_rec) ? "Iterative" : "Recursive", speed_factor);
  } else {
    printf("  - Negligible speed difference\n");
  }

  printf("  - Recommendation: %s\n\n",
         (time_ite < time_rec) ? "Use iterative method" : "Both are valid");
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

Status read_double(double *value) {
  if (scanf("%lf", value) != 1) {
    clear_input_buffer();
    return ERR_INVALID_INPUT;
  }
  clear_input_buffer();
  return SUCCESS;
}

Result factorial_rec(int n) {
  Result res = {SUCCESS, 1.0, 1};

  if (n < 0) {
    res.status = ERR_NEGATIVE_VAL;
    res.count = 0;
    return res;
  }

  if (n <= 1) {
    return res;
  }

  Result prev = factorial_rec(n - 1);
  res.value = n * prev.value;
  res.count = prev.count + 1;

  return res;
}

Result factorial_ite(int n) {
  Result res = {SUCCESS, 1.0, 0};

  if (n < 0) {
    res.status = ERR_NEGATIVE_VAL;
    return res;
  }

  for (int i = 2; i <= n; i++) {
    res.value *= i;
    res.count++;
  }

  return res;
}

/*
 * Fibonacci Recursive Implementation
 *
 * Note: This implementation uses recursion without memoization.
 * The time complexity is O(2^n) due to redundant calculations.
 *
 * For example, fib(5) calls:
 *   fib(4) + fib(3)
 *   fib(4) calls fib(3) + fib(2)
 *   fib(3) is calculated twice, fib(2) multiple times, etc.
 *
 * This leads to exponential growth in function calls. n is limited to
 * MAX_FIBONACCI_TERM to avoid stack overflow and excessive computation time.
 */
Result fibonacci_rec(int n) {
  Result res = {SUCCESS, 0.0, 1};

  if (n < 1) {
    res.status = ERR_FIBONACCI_INVALID;
    res.count = 0;
    return res;
  }

  if (n > MAX_FIBONACCI_TERM) {
    res.status = ERR_FIBONACCI_LIMIT;
    res.count = 0;
    return res;
  }

  if (n == 1) {
    res.value = 0;
    return res;
  }

  if (n == 2) {
    res.value = 1;
    return res;
  }

  Result fib1 = fibonacci_rec(n - 1);
  Result fib2 = fibonacci_rec(n - 2);
  res.value = fib1.value + fib2.value;
  res.count = fib1.count + fib2.count + 1;

  return res;
}

Result fibonacci_ite(int n) {
  Result res = {SUCCESS, 0.0, 0};

  if (n < 1) {
    res.status = ERR_FIBONACCI_INVALID;
    return res;
  }

  if (n == 1) {
    res.value = 0;
    return res;
  }

  double a = 0, b = 1;
  res.value = 1;

  for (int i = 2; i < n; i++) {
    res.value = a + b;
    a = b;
    b = res.value;
    res.count++;
  }

  res.value = b;

  return res;
}

Result sum_natural_rec(int n) {
  Result res = {SUCCESS, 0.0, 1};

  if (n < 0) {
    res.status = ERR_NEGATIVE_VAL;
    res.count = 0;
    return res;
  }

  if (n == 0) {
    return res;
  }

  Result prev = sum_natural_rec(n - 1);
  res.value = n + prev.value;
  res.count = prev.count + 1;

  return res;
}

Result sum_natural_ite(int n) {
  Result res = {SUCCESS, 0.0, 0};

  if (n < 0) {
    res.status = ERR_NEGATIVE_VAL;
    return res;
  }

  for (int i = 1; i <= n; i++) {
    res.value += (double)i;
    res.count++;
  }

  return res;
}

Result power_rec(double base, int exp) {
  Result res = {SUCCESS, 1.0, 1};

  if (base == 0.0 && exp == 0) {
    res.status = ERR_UNDEFINED;
    res.count = 0;
    return res;
  }

  if (exp == 0) {
    return res;
  }

  if (exp < 0) {
    Result pos_result = power_rec(base, -exp);
    if (pos_result.status != SUCCESS) {
      return pos_result;
    }
    res.value = 1.0 / pos_result.value;
    res.count = pos_result.count;
    return res;
  }

  Result prev = power_rec(base, exp - 1);
  res.value = base * prev.value;
  res.count = prev.count + 1;

  return res;
}

Result power_ite(double base, int exp) {
  Result res = {SUCCESS, 1.0, 0};

  if (base == 0.0 && exp == 0) {
    res.status = ERR_UNDEFINED;
    return res;
  }

  int positive_exp = (exp < 0) ? -exp : exp;
  for (int i = 0; i < positive_exp; i++) {
    res.value *= base;
    res.count++;
  }

  if (exp < 0) {
    res.value = 1.0 / res.value;
  }

  return res;
}
