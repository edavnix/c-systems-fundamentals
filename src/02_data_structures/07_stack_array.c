/*
 ===============================================================================
 Exercise: 07_stack_array.c
 Description: Stack (LIFO) implementation using dynamic array
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Dynamic Array with initial capacity of 5
 - Automatic resizing (Growth factor: 2x)
 - O(1) Access for Push/Pop/Peek
 - Visual display (Stack structure)
 - Architectural comparison (Array vs Linked List)
 - Dynamic memory management with proper cleanup
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define INITIAL_CAPACITY 5
#define MIN_OPTION 1
#define MAX_OPTION 9

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_MEMORY_ALLOCATION,
  ERR_STACK_EMPTY
} Status;

typedef struct {
  int *data;
  int top;
  int capacity;
} Stack;

void show_menu(void);
void show_comparison(void);
void handle_error(Status status);
void run_push(Stack *stack);
void run_pop(Stack *stack);
void run_peek(const Stack *stack);
void run_check_empty(const Stack *stack);
void run_status(const Stack *stack);
void run_show(const Stack *stack);
void run_clear(Stack *stack);

void clear_input_buffer(void);
Status read_integer(int *value);

Status init_stack(Stack *stack);
Status resize_stack(Stack *stack);
Status push(Stack *stack, int value);
Status pop(Stack *stack, int *value);
Status peek(const Stack *stack, int *value);
int is_empty(const Stack *stack);
void clear_stack(Stack *stack);
void free_stack(Stack *stack);

int main(void) {
  int option = 0;
  Stack stack;

  if (init_stack(&stack) != SUCCESS) {
    printf("Fatal Error: Could not allocate initial memory.\n");
    return 1;
  }

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nExiting. Freeing memory...\n");
      free_stack(&stack);
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    switch (option) {
    case 1:
      run_push(&stack);
      break;
    case 2:
      run_pop(&stack);
      break;
    case 3:
      run_peek(&stack);
      break;
    case 4:
      run_check_empty(&stack);
      break;
    case 5:
      run_status(&stack);
      break;
    case 6:
      run_show(&stack);
      break;
    case 7:
      run_clear(&stack);
      break;
    case 8:
      show_comparison();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Stack (LIFO) - Dynamic Array ===\n\n");
  printf("1. Push (Add to top)\n2. Pop (Remove from top)\n3. Peek (View top)\n"
         "4. Check if empty\n5. View Size & Capacity\n6. Show stack\n"
         "7. Clear stack\n8. Compare: Array vs Linked List\n9. Exit\n");
  printf("Option: ");
}

void show_comparison(void) {
  printf("\n=== Comparison: Stack Implementations ===\n\n");
  printf("Array-based Stack:\n");
  printf("  + Fast access (Cache locality friendly)\n");
  printf("  + Lower memory overhead (no pointers per node)\n");
  printf("  - Fixed size initially (Requires costly resize operation)\n");
  printf("\nLinked List-based Stack:\n");
  printf("  + Unlimited dynamic size (only limited by RAM)\n");
  printf("  + No resize spikes (Consistent O(1) push)\n");
  printf("  - Higher memory usage (Extra pointer per element)\n\n");
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
    printf("Error: System memory allocation failed.\n\n");
    break;
  case ERR_STACK_EMPTY:
    printf("Error: Stack is empty.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_push(Stack *stack) {
  int value;
  printf("\nValue to push: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Status status = push(stack, value);
  if (status == SUCCESS) {
    printf("\n  - [%d] pushed. Size: %d/%d\n\n", value, stack->top + 1,
           stack->capacity);
  } else {
    handle_error(status);
  }
}

void run_pop(Stack *stack) {
  int value;
  Status status = pop(stack, &value);

  if (status == SUCCESS) {
    printf("\n  - Popped element: %d\n", value);
    if (stack->top > -1) {
      printf("  - New top: %d\n\n", stack->data[stack->top]);
    } else {
      printf("  - Stack is now empty.\n\n");
    }
  } else {
    handle_error(status);
  }
}

void run_peek(const Stack *stack) {
  int value;
  Status status = peek(stack, &value);

  if (status == SUCCESS) {
    printf("\n  - Current top: %d (Index: %d)\n\n", value, stack->top);
  } else {
    handle_error(status);
  }
}

void run_check_empty(const Stack *stack) {
  if (is_empty(stack)) {
    printf("\n  - Status: Stack is EMPTY.\n\n");
  } else {
    printf("\n  - Status: Stack has %d items.\n\n", stack->top + 1);
  }
}

void run_status(const Stack *stack) {
  printf("\nStack Statistics:\n");
  printf("  - Size (Used):      %d\n", stack->top + 1);
  printf("  - Capacity (Total): %d\n", stack->capacity);
  printf("  - Available:        %d\n\n", stack->capacity - (stack->top + 1));
}

void run_show(const Stack *stack) {
  if (is_empty(stack)) {
    printf("\n  - Stack is empty.\n\n");
    return;
  }

  printf("\nStack (Top to Bottom):\n");
  for (int i = stack->top; i >= 0; i--) {
    if (i == stack->top) {
      printf("  [%d] <- TOP\n", stack->data[i]);
    } else {
      printf("  [%d]\n", stack->data[i]);
    }
  }
  printf("  (Base)\n");
  printf("  - Size: %d/%d\n\n", stack->top + 1, stack->capacity);
}

void run_clear(Stack *stack) {
  clear_stack(stack);
  printf("\n  - Stack cleared (Indices reset, memory retained).\n\n");
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

Status init_stack(Stack *stack) {
  stack->data = (int *)malloc(INITIAL_CAPACITY * sizeof(int));
  if (stack->data == NULL) {
    return ERR_MEMORY_ALLOCATION;
  }

  stack->capacity = INITIAL_CAPACITY;
  stack->top = -1;
  return SUCCESS;
}

Status resize_stack(Stack *stack) {
  printf("  ⚠ Stack full (Cap: %d). Resizing...\n", stack->capacity);

  int new_capacity = stack->capacity * 2;
  int *new_data = (int *)realloc(stack->data, new_capacity * sizeof(int));

  if (new_data == NULL) {
    return ERR_MEMORY_ALLOCATION;
  }

  stack->data = new_data;
  stack->capacity = new_capacity;

  printf("  ✓ New capacity: %d slots\n", stack->capacity);
  return SUCCESS;
}

Status push(Stack *stack, int value) {
  if (stack->top + 1 >= stack->capacity) {
    if (resize_stack(stack) != SUCCESS) {
      return ERR_MEMORY_ALLOCATION;
    }
  }

  stack->data[++stack->top] = value;
  return SUCCESS;
}

Status pop(Stack *stack, int *value) {
  if (is_empty(stack)) {
    return ERR_STACK_EMPTY;
  }

  *value = stack->data[stack->top--];
  return SUCCESS;
}

Status peek(const Stack *stack, int *value) {
  if (is_empty(stack)) {
    return ERR_STACK_EMPTY;
  }

  *value = stack->data[stack->top];
  return SUCCESS;
}

int is_empty(const Stack *stack) { return (stack->top == -1); }

void clear_stack(Stack *stack) { stack->top = -1; }

void free_stack(Stack *stack) {
  if (stack->data != NULL) {
    free(stack->data);
    stack->data = NULL;
  }
  stack->top = -1;
  stack->capacity = 0;
}
