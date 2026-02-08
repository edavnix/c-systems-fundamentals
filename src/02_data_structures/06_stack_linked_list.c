/*
 ===============================================================================
 Exercise: 06_stack_linked_list.c
 Description: Stack (LIFO) implementation using linked list
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Dynamic memory management (No fixed array size)
 - Standard Operations: Push, Pop, Peek
 - Status checks: IsEmpty, Size
 - Visual display (Top to Bottom)
 - Dynamic memory management with proper cleanup
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 8

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_MEMORY_ALLOCATION,
  ERR_STACK_EMPTY
} Status;

typedef struct Node {
  int value;
  struct Node *next;
} Node;

typedef struct {
  Node *top;
  int count;
} Stack;

void show_menu(void);
void handle_error(Status status);
void run_push(Stack *stack);
void run_pop(Stack *stack);
void run_peek(const Stack *stack);
void run_check_empty(const Stack *stack);
void run_size(const Stack *stack);
void run_show(const Stack *stack);
void run_clear(Stack *stack);

void clear_input_buffer(void);
Status read_integer(int *value);

Status init_stack(Stack *stack);
Status push(Stack *stack, int value);
Status pop(Stack *stack, int *value);
Status peek(const Stack *stack, int *value);
int is_empty(const Stack *stack);
void clear_stack(Stack *stack);

int main(void) {
  int option = 0;
  Stack stack;

  init_stack(&stack);

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nExiting stack manager. Cleaning up...\n");
      clear_stack(&stack);
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
      run_size(&stack);
      break;
    case 6:
      run_show(&stack);
      break;
    case 7:
      run_clear(&stack);
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Stack (LIFO) - Linked List ===\n\n");
  printf("1. Push (Add to top)\n2. Pop (Remove from top)\n3. Peek (View top)\n"
         "4. Check if empty\n5. Get size\n6. Show stack\n7. Clear stack\n8. "
         "Exit\n");
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
    printf("\n  - [%d] pushed. Current size: %d\n\n", value, stack->count);
  } else {
    handle_error(status);
  }
}

void run_pop(Stack *stack) {
  int value;
  Status status = pop(stack, &value);

  if (status == SUCCESS) {
    printf("\n  - Popped element: %d\n", value);
    if (stack->top != NULL) {
      printf("  - New top: %d\n\n", stack->top->value);
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
    printf("\n  - Current top: %d (Not removed)\n\n", value);
  } else {
    handle_error(status);
  }
}

void run_check_empty(const Stack *stack) {
  if (is_empty(stack)) {
    printf("\n  - Status: Stack is EMPTY.\n\n");
  } else {
    printf("\n  - Status: Stack has items.\n\n");
  }
}

void run_size(const Stack *stack) {
  printf("\n  - Current stack size: %d elements\n\n", stack->count);
}

void run_show(const Stack *stack) {
  if (is_empty(stack)) {
    printf("\n  - Stack is empty.\n\n");
    return;
  }

  printf("\nStack (Top to Bottom):\n");
  Node *current = stack->top;
  int is_top = TRUE;

  while (current != NULL) {
    if (is_top) {
      printf("  [%d] <- TOP\n", current->value);
      is_top = FALSE;
    } else {
      printf("  [%d]\n", current->value);
    }
    current = current->next;
  }
  printf("  (Base)\n");
  printf("  - Size: %d elements\n\n", stack->count);
}

void run_clear(Stack *stack) {
  clear_stack(stack);
  printf("\n  - Stack cleared.\n\n");
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
  stack->top = NULL;
  stack->count = 0;
  return SUCCESS;
}

Status push(Stack *stack, int value) {
  Node *new_node = (Node *)malloc(sizeof(Node));
  if (!new_node) {
    return ERR_MEMORY_ALLOCATION;
  }

  new_node->value = value;
  new_node->next = stack->top;
  stack->top = new_node;
  stack->count++;

  return SUCCESS;
}

Status pop(Stack *stack, int *value) {
  if (is_empty(stack)) {
    return ERR_STACK_EMPTY;
  }

  Node *temp = stack->top;
  *value = temp->value;

  stack->top = stack->top->next;
  free(temp);
  stack->count--;

  return SUCCESS;
}

Status peek(const Stack *stack, int *value) {
  if (is_empty(stack)) {
    return ERR_STACK_EMPTY;
  }

  *value = stack->top->value;
  return SUCCESS;
}

int is_empty(const Stack *stack) { return (stack->top == NULL); }

void clear_stack(Stack *stack) {
  Node *current = stack->top;
  Node *next_node;

  while (current != NULL) {
    next_node = current->next;
    free(current);
    current = next_node;
  }

  stack->top = NULL;
  stack->count = 0;
}
