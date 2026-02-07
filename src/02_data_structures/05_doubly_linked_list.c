/*
 ===============================================================================
 Exercise: 05_doubly_linked_list.c
 Description: Doubly Linked List Manager with bidirectional navigation
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Doubly Linked Nodes (prev/next pointers)
 - Head and Tail pointers for O(1) insertions at ends
 - Bidirectional Traversal (Forward/Backward display)
 - Interactive Navigation Mode (Next/Prev step-by-step)
 - List Reversal (Pointer swapping)
 - Dynamic memory management with proper cleanup
 ===============================================================================
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 11

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_MEMORY_ALLOCATION,
  ERR_LIST_EMPTY,
  ERR_NOT_FOUND,
  ERR_INVALID_POSITION
} Status;

typedef struct Node {
  int value;
  struct Node *next;
  struct Node *prev;
} Node;

typedef struct {
  Node *head;
  Node *tail;
  int count;
} DoublyLinkedList;

void show_menu(void);
void handle_error(Status status);
void run_insert_front(DoublyLinkedList *list);
void run_insert_back(DoublyLinkedList *list);
void run_insert_pos(DoublyLinkedList *list);
void run_delete_val(DoublyLinkedList *list);
void run_search(const DoublyLinkedList *list);
void run_reverse(DoublyLinkedList *list);
void run_count(const DoublyLinkedList *list);
void run_show(const DoublyLinkedList *list);
void run_navigate(const DoublyLinkedList *list);
void run_clear(DoublyLinkedList *list);

void clear_input_buffer(void);
Status read_integer(int *value);
Status read_char(char *value);

Status init_list(DoublyLinkedList *list);
Status insert_front(DoublyLinkedList *list, int value);
Status insert_back(DoublyLinkedList *list, int value);
Status insert_at(DoublyLinkedList *list, int value, int position);
Status delete_value(DoublyLinkedList *list, int value);
int search_node(const DoublyLinkedList *list, int value);
Status reverse_list(DoublyLinkedList *list);
void clear_list(DoublyLinkedList *list);

int main(void) {
  int option = 0;
  DoublyLinkedList list;

  init_list(&list);

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nExiting manager. Cleaning up memory...\n");
      clear_list(&list);
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    switch (option) {
    case 1:
      run_insert_front(&list);
      break;
    case 2:
      run_insert_back(&list);
      break;
    case 3:
      run_insert_pos(&list);
      break;
    case 4:
      run_delete_val(&list);
      break;
    case 5:
      run_search(&list);
      break;
    case 6:
      run_reverse(&list);
      break;
    case 7:
      run_count(&list);
      break;
    case 8:
      run_show(&list);
      break;
    case 9:
      run_clear(&list);
      break;
    case 10:
      run_navigate(&list);
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Doubly Linked List Manager ===\n\n");
  printf("1. Insert at beginning\n2. Insert at end\n3. Insert at position\n"
         "4. Delete by value\n5. Search element\n6. Reverse list\n"
         "7. Count elements\n8. Show list (Forward/Backward)\n"
         "9. Clear list\n10. Navigate list (Interactive)\n11. Exit\n");
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
  case ERR_LIST_EMPTY:
    printf("Error: List is empty.\n\n");
    break;
  case ERR_NOT_FOUND:
    printf("Error: Value not found in list.\n\n");
    break;
  case ERR_INVALID_POSITION:
    printf("Error: Invalid position index.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_insert_front(DoublyLinkedList *list) {
  int value;
  printf("\nValue: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Status status = insert_front(list, value);
  if (status == SUCCESS) {
    printf("\n  - Node inserted at beginning.\n\n");
  } else {
    handle_error(status);
  }
}

void run_insert_back(DoublyLinkedList *list) {
  int value;
  printf("\nValue: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Status status = insert_back(list, value);
  if (status == SUCCESS) {
    printf("\n  - Node inserted at end.\n\n");
  } else {
    handle_error(status);
  }
}

void run_insert_pos(DoublyLinkedList *list) {
  int value, pos;
  printf("\nValue: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("Position (0 to %d): ", list->count);
  if (read_integer(&pos) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Status status = insert_at(list, value, pos);
  if (status == SUCCESS) {
    printf("\n  - Node inserted at index %d.\n\n", pos);
  } else {
    handle_error(status);
  }
}

void run_delete_val(DoublyLinkedList *list) {
  int value;
  printf("\nValue to delete: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Status status = delete_value(list, value);
  if (status == SUCCESS) {
    printf("\n  - Node with value %d deleted.\n\n", value);
  } else {
    handle_error(status);
  }
}

void run_search(const DoublyLinkedList *list) {
  int value;
  printf("\nSearch value: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  int index = search_node(list, value);
  if (index != -1) {
    printf("\n  - Found at index: %d\n\n", index);
  } else {
    handle_error(ERR_NOT_FOUND);
  }
}

void run_reverse(DoublyLinkedList *list) {
  if (list->head == NULL) {
    handle_error(ERR_LIST_EMPTY);
    return;
  }

  reverse_list(list);
  printf("\n  - List reversed successfully.\n\n");
}

void run_count(const DoublyLinkedList *list) {
  printf("\n  - Total elements: %d\n\n", list->count);
}

void run_show(const DoublyLinkedList *list) {
  if (list->head == NULL) {
    printf("\n  - List is empty.\n\n");
    return;
  }

  Node *temp;

  printf("\nForward Traversal:\n");
  printf("NULL <- ");
  temp = list->head;
  while (temp != NULL) {
    printf("%d", temp->value);
    if (temp->next != NULL) {
      printf(" <-> ");
    }
    temp = temp->next;
  }
  printf(" -> NULL\n");

  printf("\nBackward Traversal:\n");
  printf("NULL <- ");
  temp = list->tail;
  while (temp != NULL) {
    printf("%d", temp->value);
    if (temp->prev != NULL) {
      printf(" <-> ");
    }
    temp = temp->prev;
  }
  printf(" -> NULL\n");
  printf("  - Length: %d nodes\n\n", list->count);
}

void run_navigate(const DoublyLinkedList *list) {
  if (list->head == NULL) {
    handle_error(ERR_LIST_EMPTY);
    return;
  }

  Node *current = list->head;
  char cmd;

  printf("\n--- Navigation Mode ---\n");

  while (TRUE) {
    printf("\nCurrent Position: %d\n", current->value);
    printf("[N]ext | [P]rev | [Q]uit: ");

    if (read_char(&cmd) != SUCCESS) {
      continue;
    }
    cmd = tolower(cmd);

    if (cmd == 'q') {
      printf("\n");
      break;
    } else if (cmd == 'n') {
      if (current->next != NULL) {
        current = current->next;
      } else {
        printf(">> End of list reached.\n");
      }
    } else if (cmd == 'p') {
      if (current->prev != NULL) {
        current = current->prev;
      } else {
        printf(">> Start of list reached.\n");
      }
    } else {
      printf(">> Invalid command.\n");
    }
  }
}

void run_clear(DoublyLinkedList *list) {
  clear_list(list);
  printf("\n  - List cleared.\n\n");
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

Status read_char(char *value) {
  if (scanf(" %c", value) != 1) {
    clear_input_buffer();
    return ERR_INVALID_INPUT;
  }
  clear_input_buffer();
  return SUCCESS;
}

Status init_list(DoublyLinkedList *list) {
  list->head = NULL;
  list->tail = NULL;
  list->count = 0;
  return SUCCESS;
}

Status insert_front(DoublyLinkedList *list, int value) {
  Node *new_node = (Node *)malloc(sizeof(Node));
  if (!new_node) {
    return ERR_MEMORY_ALLOCATION;
  }

  new_node->value = value;
  new_node->prev = NULL;
  new_node->next = list->head;

  if (list->head != NULL) {
    list->head->prev = new_node;
  } else {
    list->tail = new_node;
  }

  list->head = new_node;
  list->count++;
  return SUCCESS;
}

Status insert_back(DoublyLinkedList *list, int value) {
  Node *new_node = (Node *)malloc(sizeof(Node));
  if (!new_node) {
    return ERR_MEMORY_ALLOCATION;
  }

  new_node->value = value;
  new_node->next = NULL;
  new_node->prev = list->tail;

  if (list->tail != NULL) {
    list->tail->next = new_node;
  } else {
    list->head = new_node;
  }

  list->tail = new_node;
  list->count++;
  return SUCCESS;
}

Status insert_at(DoublyLinkedList *list, int value, int position) {
  if (position < 0 || position > list->count) {
    return ERR_INVALID_POSITION;
  }

  if (position == 0) {
    return insert_front(list, value);
  }

  if (position == list->count) {
    return insert_back(list, value);
  }

  Node *current = list->head;
  for (int i = 0; i < position - 1; i++) {
    current = current->next;
  }

  Node *new_node = (Node *)malloc(sizeof(Node));
  if (!new_node) {
    return ERR_MEMORY_ALLOCATION;
  }

  new_node->value = value;
  new_node->next = current->next;
  new_node->prev = current;

  if (current->next != NULL) {
    current->next->prev = new_node;
  }
  current->next = new_node;

  list->count++;
  return SUCCESS;
}

Status delete_value(DoublyLinkedList *list, int value) {
  if (list->head == NULL) {
    return ERR_LIST_EMPTY;
  }

  Node *current = list->head;

  while (current != NULL) {
    if (current->value == value) {
      if (current->prev != NULL) {
        current->prev->next = current->next;
      } else {
        list->head = current->next;
      }

      if (current->next != NULL) {
        current->next->prev = current->prev;
      } else {
        list->tail = current->prev;
      }

      free(current);
      list->count--;
      return SUCCESS;
    }
    current = current->next;
  }

  return ERR_NOT_FOUND;
}

int search_node(const DoublyLinkedList *list, int value) {
  Node *temp = list->head;
  int index = 0;
  while (temp != NULL) {
    if (temp->value == value) {
      return index;
    }
    temp = temp->next;
    index++;
  }

  return -1;
}

Status reverse_list(DoublyLinkedList *list) {
  Node *temp = NULL;
  Node *current = list->head;

  while (current != NULL) {
    temp = current->prev;
    current->prev = current->next;
    current->next = temp;
    current = current->prev;
  }

  if (temp != NULL) {
    list->tail = list->head;
    list->head = temp->prev;
  }

  return SUCCESS;
}

void clear_list(DoublyLinkedList *list) {
  Node *current = list->head;
  Node *next;

  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }

  list->head = NULL;
  list->tail = NULL;
  list->count = 0;
}
