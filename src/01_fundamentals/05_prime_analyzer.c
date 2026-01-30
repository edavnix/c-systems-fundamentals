/*
 ===============================================================================
 Exercise: 05_prime_analyzer.c
 Description: Prime number analysis tool with multiple algorithms
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Sieve of Eratosthenes implementation for range analysis
 - Primality test using trial division
 - Prime factorization
 - Twin prime identification
 - Efficient memory management for dynamic arrays
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 5
#define MAX_RANGE 10000

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_RANGE,
  ERR_MEMORY_FAILURE,
  ERR_MIN_VALUE
} Status;

typedef struct {
  Status status;
  double value;
} Result;

typedef struct {
  int factor;
  int exponent;
} PrimeFactor;

void show_menu(void);
void handle_error(Status status);
void run_list_primes(void);
void run_check_primality(void);
void run_prime_factorization(void);
void run_twin_primes(void);

void clear_input_buffer(void);
Status read_integer(int *value);

Result is_prime(int num);
Status compute_sieve(int **array, int n);
int get_prime_factors(int num, PrimeFactor factors[]);

int main(void) {
  int option = 0;

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nThank you for using the prime analyzer!\n");
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    switch (option) {
    case 1:
      run_list_primes();
      break;
    case 2:
      run_check_primality();
      break;
    case 3:
      run_prime_factorization();
      break;
    case 4:
      run_twin_primes();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Prime Number Analyzer ===\n\n");
  printf("1. List primes in range\n2. Check primality\n"
         "3. Prime factorization\n4. Twin primes\n5. Exit\n");
  printf("Option: ");
}

void handle_error(Status status) {
  switch (status) {
  case ERR_INVALID_INPUT:
    printf("Error: Invalid input. Please enter an integer.\n\n");
    break;
  case ERR_INVALID_RANGE:
    printf("Error: Invalid range. 0 <= start <= end <= %d.\n\n", MAX_RANGE);
    break;
  case ERR_MEMORY_FAILURE:
    printf("Error: Memory allocation failed.\n\n");
    break;
  case ERR_MIN_VALUE:
    printf("Error: Number must be at least 2.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_list_primes(void) {
  int start, end;

  printf("\nEnter start range: ");
  if (read_integer(&start) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("Enter end range (max %d): ", MAX_RANGE);
  if (read_integer(&end) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  if (start < 0 || end > MAX_RANGE || start > end) {
    handle_error(ERR_INVALID_RANGE);
    return;
  }

  int *is_prime_map = NULL;
  Status status = compute_sieve(&is_prime_map, end);
  if (status != SUCCESS) {
    handle_error(status);
    return;
  }

  int count = 0;
  for (int i = start; i <= end; i++) {
    if (is_prime_map[i]) {
      count++;
    }
  }

  printf("\n  - Primes found in range [%d, %d]: %d\n", start, end, count);
  printf("  - List: [");

  int first = TRUE;
  for (int i = start; i <= end; i++) {
    if (is_prime_map[i]) {
      if (!first) {
        printf(", ");
      }
      printf("%d", i);
      first = FALSE;
    }
  }
  printf("]\n\n");

  free(is_prime_map);
}

void run_check_primality(void) {
  int num;

  printf("\nEnter number: ");
  if (read_integer(&num) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Result res = is_prime(num);
  if (res.status == SUCCESS) {
    printf("\n  - Result: %s\n\n",
           (res.value > 0.5) ? "IS PRIME" : "NOT PRIME");
  } else {
    handle_error(res.status);
  }
}

void run_prime_factorization(void) {
  int num;

  printf("\nEnter number (min 2): ");
  if (read_integer(&num) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  if (num < 2) {
    handle_error(ERR_MIN_VALUE);
    return;
  }

  PrimeFactor factors[64];
  int count = get_prime_factors(num, factors);
  printf("\n  - Factorization: %d = ", num);
  for (int i = 0; i < count; i++) {
    if (i > 0) {
      printf(" x ");
    }
    printf("%d^%d", factors[i].factor, factors[i].exponent);
  }
  printf("\n\n");
}

void run_twin_primes(void) {
  int start, end;

  printf("\nEnter start range: ");
  if (read_integer(&start) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("Enter end range (max %d): ", MAX_RANGE);
  if (read_integer(&end) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  if (start < 0 || end > MAX_RANGE || start > end) {
    handle_error(ERR_INVALID_RANGE);
    return;
  }

  int *is_prime_map = NULL;
  Status status = compute_sieve(&is_prime_map, end);
  if (status != SUCCESS) {
    handle_error(status);
    return;
  }

  printf("\n  - Twin primes found: ");
  int first = TRUE;
  for (int i = start; i <= end - 2; i++) {
    if (is_prime_map[i] && is_prime_map[i + 2]) {
      if (!first) {
        printf(", ");
      }
      printf("(%d, %d)", i, i + 2);
      first = FALSE;
    }
  }

  if (first) {
    printf("None");
  }
  printf("\n\n");

  free(is_prime_map);
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

Result is_prime(int num) {
  Result res = {SUCCESS, 0.0};

  if (num < 2) {
    res.value = 0.0;
    return res;
  }

  for (int i = 2; i * i <= num; i++) {
    if (num % i == 0) {
      res.value = 0.0;
      return res;
    }
  }

  res.value = 1.0;

  return res;
}

Status compute_sieve(int **array, int n) {
  if (n < 0)
    return ERR_INVALID_RANGE;

  *array = (int *)malloc((n + 1) * sizeof(int));
  if (*array == NULL) {
    return ERR_MEMORY_FAILURE;
  }

  for (int i = 0; i <= n; i++) {
    (*array)[i] = TRUE;
  }

  (*array)[0] = (*array)[1] = FALSE;

  for (int p = 2; p * p <= n; p++) {
    if ((*array)[p] == TRUE) {
      for (int i = p * p; i <= n; i += p) {
        (*array)[i] = FALSE;
      }
    }
  }

  return SUCCESS;
}

int get_prime_factors(int num, PrimeFactor factors[]) {
  int temp = num;
  int count = 0;

  for (int i = 2; i * i <= temp; i++) {
    if (temp % i == 0) {
      factors[count].factor = i;
      factors[count].exponent = 0;
      while (temp % i == 0) {
        factors[count].exponent++;
        temp /= i;
      }
      count++;
    }
  }

  if (temp > 1) {
    factors[count].factor = temp;
    factors[count].exponent = 1;
    count++;
  }

  return count;
}
