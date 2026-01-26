/*
 ===============================================================================
 Exercise: 01_multi_calculator.c
 Description: Scientific calculator with interactive menu
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Basic arithmetic operations (add, subtract, multiply, divide)
 - Advanced operations (power, square root, factorial)
 - Input validation and error handling
 - Division by zero detection
 - Negative number validation for sqrt and factorial
 - Factorial limit checking (max 170)
 - Uses Newton-Raphson method for square root approximation
 ===============================================================================
*/

#include <stdio.h>

#define TRUE 1
#define MIN_OPTION 1
#define MAX_OPTION 8
#define SQRT_ITERATIONS 20
#define MAX_FACTORIAL 170

typedef enum {
  SUCCESS,
  ERR_DIV_ZERO,
  ERR_NEGATIVE_SQRT,
  ERR_NEGATIVE_FACTORIAL,
  ERR_FACTORIAL_LIMIT,
  ERR_INVALID_OPTION,
  ERR_INVALID_INPUT,
  ERR_UNDEFINED
} Status;

typedef struct {
  Status status;
  double value;
} Result;

void show_menu(void);
void handle_error(Status status);
void run_basic_operation(int option);
void run_power_operation(void);
void run_sqrt_operation(void);
void run_factorial_operation(void);

void clear_input_buffer(void);
Status read_integer(int *value);
Status read_double(double *value);
Status read_two_numbers(double *num1, double *num2);

Result basic_operation(int option, double num1, double num2);
Result power(double base, int exponent);
Result sqroot(double num);
Result factorial(int num);

int main(void) {
  int option = 0;

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nThank you for using the calculator!\n");
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    switch (option) {
    case 1:
    case 2:
    case 3:
    case 4:
      run_basic_operation(option);
      break;
    case 5:
      run_power_operation();
      break;
    case 6:
      run_sqrt_operation();
      break;
    case 7:
      run_factorial_operation();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("===== Scientific Calculator =====\n\n");
  printf("1. Addition\n2. Subtract\n3. Multiply\n4. Divide\n"
         "5. Power\n6. Square Root\n7. Factorial\n8. Exit\n");
  printf("Select an option: ");
}

void handle_error(Status status) {
  switch (status) {
  case ERR_DIV_ZERO:
    printf("Error: Cannot divide by zero.\n\n");
    break;
  case ERR_NEGATIVE_SQRT:
    printf("Error: Negative numbers not allowed for this operation.\n\n");
    break;
  case ERR_NEGATIVE_FACTORIAL:
    printf("Error: Negative numbers not allowed for this operation.\n\n");
    break;
  case ERR_FACTORIAL_LIMIT:
    printf("Error: Number too large for factorial (max 170).\n\n");
    break;
  case ERR_INVALID_OPTION:
    printf("Error: Invalid operation.\n\n");
    break;
  case ERR_INVALID_INPUT:
    printf("Error: Invalid input. Please enter valid numbers.\n\n");
    break;
  case ERR_UNDEFINED:
    printf("Error: Mathematical operation is undefined (0^0).\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_basic_operation(int option) {
  double num1, num2;

  Status status = read_two_numbers(&num1, &num2);
  if (status != SUCCESS) {
    handle_error(status);
    return;
  }

  Result res = basic_operation(option, num1, num2);
  if (res.status == SUCCESS) {
    printf("\n  - Result: %.2f\n\n", res.value);
  } else {
    handle_error(res.status);
  }
}

void run_power_operation(void) {
  double base;
  int exponent;
  Status status;

  printf("\nEnter base: ");
  status = read_double(&base);
  if (status != SUCCESS) {
    handle_error(status);
    return;
  }

  printf("Enter exponent (integer): ");
  status = read_integer(&exponent);
  if (status != SUCCESS) {
    handle_error(status);
    return;
  }

  Result res = power(base, exponent);
  if (res.status == SUCCESS) {
    printf("\n  - Result: %.2f\n\n", res.value);
  } else {
    handle_error(res.status);
  }
}

void run_sqrt_operation(void) {
  double num;

  printf("\nEnter number: ");
  Status status = read_double(&num);
  if (status != SUCCESS) {
    handle_error(status);
    return;
  }

  Result res = sqroot(num);
  if (res.status == SUCCESS) {
    printf("\n  - Result: %.4f\n\n", res.value);
  } else {
    handle_error(res.status);
  }
}

void run_factorial_operation(void) {
  double num;

  printf("\nEnter number: ");
  Status status = read_double(&num);
  if (status != SUCCESS) {
    handle_error(status);
    return;
  }

  Result res = factorial((int)num);
  if (res.status == SUCCESS) {
    printf("\n  - Result: %.0f\n\n", res.value);
  } else {
    handle_error(res.status);
  }
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

Status read_two_numbers(double *num1, double *num2) {
  printf("\nEnter first number: ");
  if (read_double(num1) != SUCCESS) {
    return ERR_INVALID_INPUT;
  }

  printf("Enter second number: ");
  if (read_double(num2) != SUCCESS) {
    return ERR_INVALID_INPUT;
  }

  return SUCCESS;
}

Result basic_operation(int option, double num1, double num2) {
  Result res = {SUCCESS, 0.0};

  if (option == 1) {
    res.value = num1 + num2;
  } else if (option == 2) {
    res.value = num1 - num2;
  } else if (option == 3) {
    res.value = num1 * num2;
  } else if (option == 4) {
    if (num2 == 0.0) {
      res.status = ERR_DIV_ZERO;
    } else {
      res.value = num1 / num2;
    }
  } else {
    res.status = ERR_INVALID_OPTION;
  }

  return res;
}

Result power(double base, int exponent) {
  Result res = {SUCCESS, 0.0};

  if (base == 0.0 && exponent == 0) {
    res.status = ERR_UNDEFINED;
    return res;
  }

  double val = 1.0;
  int positive_exp = (exponent < 0) ? -exponent : exponent;
  for (int i = 0; i < positive_exp; i++) {
    val *= base;
  }

  res.value = (exponent < 0) ? 1.0 / val : val;

  return res;
}

Result sqroot(double num) {
  Result res = {SUCCESS, 0.0};

  if (num < 0.0) {
    res.status = ERR_NEGATIVE_SQRT;
    return res;
  }

  if (num == 0.0) {
    return res;
  }

  double val = num;
  for (int i = 0; i < SQRT_ITERATIONS; i++) {
    val = 0.5 * (val + num / val);
  }

  res.value = val;

  return res;
}

Result factorial(int num) {
  Result res = {SUCCESS, 0.0};

  if (num < 0) {
    res.status = ERR_NEGATIVE_FACTORIAL;
    return res;
  }

  if (num > MAX_FACTORIAL) {
    res.status = ERR_FACTORIAL_LIMIT;
    return res;
  }

  double val = 1.0;
  for (int i = 2; i <= num; i++) {
    val *= i;
  }

  res.value = val;

  return res;
}
