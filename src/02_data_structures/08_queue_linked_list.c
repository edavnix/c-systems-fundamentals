/*
 ===============================================================================
 Exercise: 08_queue_linked_list.c
 Description: Queue (FIFO) implementation using linked list
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - O(1) Enqueue (Insertion at Rear)
 - O(1) Dequeue (Removal from Front)
 - Visual display (Front -> Rear)
 - Practical use cases: Buffer management, BFS, Scheduling
 - Dynamic memory management with proper cleanup
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 9

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_MEMORY_ALLOCATION,
  ERR_QUEUE_EMPTY
} Status;

typedef struct Node {
  int value;
  struct Node *next;
} Node;

typedef struct {
  Node *front;
  Node *rear;
  int count;
} Queue;

void show_menu(void);
void handle_error(Status status);
void run_enqueue(Queue *q);
void run_dequeue(Queue *q);
void run_peek_front(const Queue *q);
void run_peek_rear(const Queue *q);
void run_check_empty(const Queue *q);
void run_size(const Queue *q);
void run_show(const Queue *q);
void run_clear(Queue *q);

void clear_input_buffer(void);
Status read_integer(int *value);

Status init_queue(Queue *q);
Status enqueue(Queue *q, int value);
Status dequeue(Queue *q, int *value);
Status peek_front(const Queue *q, int *value);
Status peek_rear(const Queue *q, int *value);
int is_empty(const Queue *q);
void clear_queue(Queue *q);

int main(void) {
  int option = 0;
  Queue q;

  init_queue(&q);

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nExiting queue manager. Cleaning up...\n");
      clear_queue(&q);
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
      run_peek_front(&q);
      break;
    case 4:
      run_peek_rear(&q);
      break;
    case 5:
      run_check_empty(&q);
      break;
    case 6:
      run_size(&q);
      break;
    case 7:
      run_show(&q);
      break;
    case 8:
      run_clear(&q);
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Queue (FIFO) - Linked List ===\n\n");
  printf("1. Enqueue (Add to rear)\n2. Dequeue (Remove from front)\n"
         "3. Peek Front\n4. Peek Rear\n5. Check if empty\n"
         "6. Get size\n7. Show queue\n8. Clear queue\n9. Exit\n");
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
  case ERR_QUEUE_EMPTY:
    printf("Error: Queue is empty.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_enqueue(Queue *q) {
  int value;
  printf("\nValue to enqueue: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Status status = enqueue(q, value);
  if (status == SUCCESS) {
    printf("\n  - [%d] enqueued. Size: %d\n\n", value, q->count);
  } else {
    handle_error(status);
  }
}

void run_dequeue(Queue *q) {
  int value;
  Status status = dequeue(q, &value);

  if (status == SUCCESS) {
    printf("\n  - Dequeued element: %d\n", value);
    if (!is_empty(q)) {
      printf("  - New front: %d\n\n", q->front->value);
    } else {
      printf("  - Queue is now empty.\n\n");
    }
  } else {
    handle_error(status);
  }
}

void run_peek_front(const Queue *q) {
  int value;
  Status status = peek_front(q, &value);

  if (status == SUCCESS) {
    printf("\n  - Front element: %d\n\n", value);
  } else {
    handle_error(status);
  }
}

void run_peek_rear(const Queue *q) {
  int value;
  Status status = peek_rear(q, &value);

  if (status == SUCCESS) {
    printf("\n  - Rear element: %d\n\n", value);
  } else {
    handle_error(status);
  }
}

void run_check_empty(const Queue *q) {
  if (is_empty(q)) {
    printf("\n  - Status: Queue is EMPTY.\n\n");
  } else {
    printf("\n  - Status: Queue has items.\n\n");
  }
}

void run_size(const Queue *q) {
  printf("\n  - Queue size: %d elements\n\n", q->count);
}

void run_show(const Queue *q) {
  if (is_empty(q)) {
    printf("\n  - Queue is empty.\n\n");
    return;
  }

  printf("\nQueue (Front to Rear):\n");
  printf("FRONT -> ");

  Node *temp = q->front;
  while (temp != NULL) {
    printf("[%d]", temp->value);
    if (temp->next != NULL) {
      printf(" -> ");
    }
    temp = temp->next;
  }

  printf(" <- REAR\n");
  printf("  - Size: %d elements\n\n", q->count);
}

void run_clear(Queue *q) {
  clear_queue(q);
  printf("\n  - Queue cleared.\n\n");
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

Status init_queue(Queue *q) {
  q->front = NULL;
  q->rear = NULL;
  q->count = 0;

  return SUCCESS;
}

Status enqueue(Queue *q, int value) {
  Node *new_node = (Node *)malloc(sizeof(Node));
  if (!new_node) {
    return ERR_MEMORY_ALLOCATION;
  }

  new_node->value = value;
  new_node->next = NULL;

  if (q->rear == NULL) {
    q->front = new_node;
    q->rear = new_node;
  } else {
    q->rear->next = new_node;
    q->rear = new_node;
  }

  q->count++;

  return SUCCESS;
}

Status dequeue(Queue *q, int *value) {
  if (is_empty(q)) {
    return ERR_QUEUE_EMPTY;
  }

  Node *temp = q->front;
  *value = temp->value;

  q->front = q->front->next;

  if (q->front == NULL) {
    q->rear = NULL;
  }

  free(temp);
  q->count--;

  return SUCCESS;
}

Status peek_front(const Queue *q, int *value) {
  if (is_empty(q)) {
    return ERR_QUEUE_EMPTY;
  }

  *value = q->front->value;

  return SUCCESS;
}

Status peek_rear(const Queue *q, int *value) {
  if (is_empty(q)) {
    return ERR_QUEUE_EMPTY;
  }

  *value = q->rear->value;

  return SUCCESS;
}

int is_empty(const Queue *q) { return (q->front == NULL); }

void clear_queue(Queue *q) {
  Node *current = q->front;
  Node *next;

  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }

  q->front = NULL;
  q->rear = NULL;
  q->count = 0;
}
