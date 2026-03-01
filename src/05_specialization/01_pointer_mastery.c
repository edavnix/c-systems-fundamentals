/*
 ===============================================================================
 Exercise: 01_pointer_mastery.c
 Description: Advanced Pointer Demonstrations
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Memory address inspection (%p)
 - Pointer arithmetic and byte offsets
 - Arrays and pointers equivalence
 - Dynamic function dispatching via function pointers
 - Void (generic) pointers and casting
 - Double and triple pointers
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 7

typedef enum { SUCCESS, ERR_INVALID_INPUT, ERR_INVALID_OPTION } Status;

void show_menu(void);
void handle_error(Status status);
void run_demo_basic(void);
void run_demo_arithmetic(void);
void run_demo_array_pointers(void);
void run_demo_func_pointers(void);
void run_demo_void_pointers(void);
void run_demo_multi_pointers(void);

void clear_input_buffer(void);
Status read_integer(int *value);

int suma(int a, int b) { return a + b; }
int resta(int a, int b) { return a - b; }
int producto(int a, int b) { return a * b; }

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
      run_demo_basic();
      break;
    case 2:
      run_demo_arithmetic();
      break;
    case 3:
      run_demo_array_pointers();
      break;
    case 4:
      run_demo_func_pointers();
      break;
    case 5:
      run_demo_void_pointers();
      break;
    case 6:
      run_demo_multi_pointers();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== C Pointer Mastery ===\n\n");
  printf("1. Basic Pointers & Addresses\n");
  printf("2. Pointer Arithmetic\n");
  printf("3. Pointers to Arrays\n");
  printf("4. Function Pointers\n");
  printf("5. Void (Generic) Pointers\n");
  printf("6. Double & Triple Pointers\n");
  printf("7. Exit\n");
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
  case SUCCESS:
    break;
  }
}

void run_demo_basic(void) {
  int x = 42;
  int *ptr = &x;

  printf("\n=== 1. Basic Pointers ===\n");
  printf("  int x = 42;\n  int *ptr = &x;\n\n");

  printf("  - Address of x (&x): %p\n", (void *)&x);
  printf("  - Value of x:        %d\n", x);

  printf("  - ptr holds address: %p\n", (void *)ptr);
  printf("  - Value at *ptr:     %d\n\n", *ptr);

  printf("  Modifying *ptr = 100...\n");
  *ptr = 100;

  printf("  - New value of x:    %d ✓\n", x);
  printf("  (A pointer modifies the underlying original variable)\n\n");
}

void run_demo_arithmetic(void) {
  int array[] = {10, 20, 30, 40, 50};
  int *ptr = array;

  printf("\n=== 2. Pointer Arithmetic ===\n");
  printf("  int array[] = {10, 20, 30, 40, 50};\n");
  printf("  int *ptr = array;\n\n");

  printf("  - Base Address:      %p\n", (void *)array);
  printf("  - ptr points to 1st element.\n\n");

  printf("  *ptr = %d, Address: %p\n", *ptr, (void *)ptr);

  printf("  Advancing (ptr + 1)...\n");
  printf("  *(ptr+1) = %d, Address: %p (+%zu bytes)\n", *(ptr + 1),
         (void *)(ptr + 1), sizeof(int));

  printf("  Advancing (ptr + 2)...\n");
  printf("  *(ptr+2) = %d, Address: %p (+%zu bytes)\n\n", *(ptr + 2),
         (void *)(ptr + 2), sizeof(int) * 2);

  printf(
      "  Note: Adding 1 to a pointer advances it by sizeof(type) bytes.\n\n");
}

void run_demo_array_pointers(void) {
  int array[] = {1, 2, 3, 4, 5};
  int (*ptr_to_array)[5] = &array; // Pointer to the entire array of 5 ints
  int *ptr_to_first = array;       // Pointer to the first element

  printf("\n=== 3. Pointers to Arrays ===\n");
  printf("  int array[5] = {1, 2, 3, 4, 5};\n");
  printf("  int *ptr_to_first = array;\n");
  printf("  int (*ptr_to_array)[5] = &array;\n\n");

  printf("  - ptr_to_first:      %p\n", (void *)ptr_to_first);
  printf("  - ptr_to_array:      %p (Same numeric address)\n\n",
         (void *)ptr_to_array);

  printf("  Behavior when adding 1:\n");
  printf("  - ptr_to_first + 1:  %p (+%zu bytes, moves 1 int)\n",
         (void *)(ptr_to_first + 1), sizeof(int));
  printf("  - ptr_to_array + 1:  %p (+%zu bytes, moves 1 whole array of 5 "
         "ints)\n\n",
         (void *)(ptr_to_array + 1), sizeof(int) * 5);
}

void run_demo_func_pointers(void) {
  int (*operacion)(int, int);

  printf("\n=== 4. Function Pointers ===\n");
  printf("  Available Functions:\n");
  printf("  - int suma(int a, int b)\n");
  printf("  - int resta(int a, int b)\n");
  printf("  - int producto(int a, int b)\n\n");

  printf("  Declaring: int (*operacion)(int, int);\n\n");

  operacion = suma;
  printf("  > operacion = suma;\n");
  printf("    Result operacion(5, 3): %d\n\n", operacion(5, 3));

  operacion = producto;
  printf("  > operacion = producto;\n");
  printf("    Result operacion(5, 3): %d\n\n", operacion(5, 3));

  printf("  Use case: Callbacks, dynamic dispatch, strategy pattern.\n\n");
}

void run_demo_void_pointers(void) {
  int i = 100;
  float f = 3.14f;
  char c = 'A';

  void *ptr;

  printf("\n=== 5. Void (Generic) Pointers ===\n");
  printf("  void *ptr can point to any data type.\n\n");

  ptr = &i;
  printf("  > Pointing to int (100):\n");
  // Cast before dereferencing
  printf("    Address: %p | Value: %d\n", ptr, *(int *)ptr);

  ptr = &f;
  printf("  > Pointing to float (3.14):\n");
  printf("    Address: %p | Value: %.2f\n", ptr, *(float *)ptr);

  ptr = &c;
  printf("  > Pointing to char ('A'):\n");
  printf("    Address: %p | Value: %c\n\n", ptr, *(char *)ptr);

  printf("  Note: Void pointers need strict casting before dereferencing.\n\n");
}

void run_demo_multi_pointers(void) {
  int x = 42;
  int *p = &x;
  int **pp = &p;
  int ***ppp = &pp;

  printf("\n=== 6. Double & Triple Pointers ===\n");
  printf("  int x = 42;         (Address: %p)\n", (void *)&x);
  printf("  int *p = &x;        (Address: %p)\n", (void *)&p);
  printf("  int **pp = &p;      (Address: %p)\n", (void *)&pp);
  printf("  int ***ppp = &pp;   (Address: %p)\n\n", (void *)&ppp);

  printf("  Tracing values through indirection:\n");
  printf("  - Deref level 1 (*p):     %d\n", *p);
  printf("  - Deref level 2 (**pp):   %d\n", **pp);
  printf("  - Deref level 3 (***ppp): %d\n\n", ***ppp);

  printf("  Modifying through triple pointer (***ppp = 999):\n");
  ***ppp = 999;

  printf("  - New value of x:         %d ✓\n\n", x);
  printf("  Use case: Modifying pointers inside functions (pass by reference "
         "for pointers).\n\n");
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
