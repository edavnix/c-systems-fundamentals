/*
 ===============================================================================
 Exercise: 09_queue_array.c
 Description: Circular Queue implementation using fixed array
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Fixed Capacity (5 slots) to demonstrate wrapping easily
 - Circular Indexing: i = (i + 1) % Capacity
 - Visual Debug View: Shows raw array state [_, _, 10, 20, _]
 - Efficient Memory Use: Reuses freed slots at the beginning of the array
 - Dynamic memory management with proper cleanup
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define CAPACITY 5
#define MIN_OPTION 1
#define MAX_OPTION 7

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_QUEUE_EMPTY,
  ERR_QUEUE_FULL
} Status;

typedef struct {
  int data[CAPACITY];
  int front;
  int rear;
  int count;
} CircularQueue;

void show_menu(void);
void handle_error(Status status);
void run_enqueue(CircularQueue *q);
void run_dequeue(CircularQueue *q);
void run_peek(const CircularQueue *q);
void run_status(const CircularQueue *q);
void run_reset(CircularQueue *q);

void clear_input_buffer(void);
Status read_integer(int *value);

void init_queue(CircularQueue *q);
Status enqueue(CircularQueue *q, int value);
Status dequeue(CircularQueue *q, int *value);
Status peek(const CircularQueue *q, int *value);
int is_empty(const CircularQueue *q);
int is_full(const CircularQueue *q);
int is_index_occupied(const CircularQueue *q, int index);

int main(void) {
  int option = 0;
  CircularQueue q;

  init_queue(&q);

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
      run_enqueue(&q);
      break;
    case 2:
      run_dequeue(&q);
      break;
    case 3:
      run_peek(&q);
      break;
    case 4:
      run_status(&q);
      break;
    case 5:
      run_reset(&q);
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Queue (FIFO) - Circular Array ===\n");
  printf("Capacity: %d\n\n", CAPACITY);
  printf("1. Enqueue\n2. Dequeue\n3. Peek Front\n"
         "4. Show Internal State (Debug)\n5. Reset Queue\n6. Exit\n");
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
  case ERR_QUEUE_FULL:
    printf("Error: Queue is FULL. Cannot enqueue.\n\n");
    break;
  case ERR_QUEUE_EMPTY:
    printf("Error: Queue is EMPTY.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_enqueue(CircularQueue *q) {
  int value;
  printf("\nValue to enqueue: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Status status = enqueue(q, value);
  if (status == SUCCESS) {
    printf("\n  - [%d] enqueued at circular index %d\n\n", value, q->rear);
    run_status(q);
  } else {
    handle_error(status);
  }
}

void run_dequeue(CircularQueue *q) {
  int value;
  int prev_front = q->front;
  Status status = dequeue(q, &value);

  if (status == SUCCESS) {
    printf("\n  - Dequeued element: %d (from index %d)\n\n", value, prev_front);
    run_status(q);
  } else {
    handle_error(status);
  }
}

void run_peek(const CircularQueue *q) {
  int value;
  Status status = peek(q, &value);

  if (status == SUCCESS) {
    printf("\n  - Front element: %d\n\n", value);
  } else {
    handle_error(status);
  }
}

void run_status(const CircularQueue *q) {
  printf("\n--- Internal State ---\n");

  printf("Array: [");
  for (int i = 0; i < CAPACITY; i++) {
    if (is_index_occupied(q, i)) {
      printf("%d", q->data[i]);
    } else {
      printf("_");
    }

    if (i < CAPACITY - 1) {
      printf(", ");
    }
  }
  printf("]\n");

  printf("Front: %d | Rear: %d | Size: %d/%d\n", q->front, q->rear, q->count,
         CAPACITY);

  if (is_full(q)) {
    printf("(Queue is Full)\n");
  }
  printf("\n");
}

void run_reset(CircularQueue *q) {
  init_queue(q);
  printf("\n  - Queue reset successfully.\n\n");
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

void init_queue(CircularQueue *q) {
  q->front = 0;
  q->rear = -1;
  q->count = 0;

  for (int i = 0; i < CAPACITY; i++) {
    q->data[i] = 0;
  }
}

int is_full(const CircularQueue *q) { return (q->count == CAPACITY); }

int is_empty(const CircularQueue *q) { return (q->count == 0); }

Status enqueue(CircularQueue *q, int value) {
  if (is_full(q)) {
    return ERR_QUEUE_FULL;
  }

  q->rear = (q->rear + 1) % CAPACITY;
  q->data[q->rear] = value;
  q->count++;

  return SUCCESS;
}

Status dequeue(CircularQueue *q, int *value) {
  if (is_empty(q)) {
    return ERR_QUEUE_EMPTY;
  }

  *value = q->data[q->front];
  q->front = (q->front + 1) % CAPACITY;
  q->count--;

  if (q->count == 0) {
    q->front = 0;
    q->rear = -1;
  }

  return SUCCESS;
}

Status peek(const CircularQueue *q, int *value) {
  if (is_empty(q)) {
    return ERR_QUEUE_EMPTY;
  }

  *value = q->data[q->front];

  return SUCCESS;
}

int is_index_occupied(const CircularQueue *q, int index) {
  if (is_empty(q)) {
    return FALSE;
  }

  if (q->rear >= q->front) {
    return (index >= q->front && index <= q->rear);
  } else {
    return (index >= q->front || index <= q->rear);
  }
}
