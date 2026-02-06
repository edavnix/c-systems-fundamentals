/*
 ===============================================================================
 Exercise: 04_singly_linked_list.c
 Description: Singly Linked List Manager with dynamic memory
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Insertions (Front, Back, Specific Position)
 - Deletion (By value)
 - Search and Count
 - List Reversal (In-place)
 - Visual display (Value -> Value -> NULL)
 - Dynamic memory management with proper cleanup
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 10

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
} Node;

void show_menu(void);
void handle_error(Status status);
void run_insert_front(Node **head);
void run_insert_back(Node **head);
void run_insert_pos(Node **head);
void run_delete_val(Node **head);
void run_search(Node *head);
void run_reverse(Node **head);
void run_count(Node *head);
void run_show(Node *head);
void run_clear(Node **head);

void clear_input_buffer(void);
Status read_integer(int *value);

Status insert_front(Node **head, int value);
Status insert_back(Node **head, int value);
Status insert_at(Node **head, int value, int position);
Status delete_value(Node **head, int value);
int search_node(Node *head, int value);
Status reverse_list(Node **head);
int count_nodes(Node *head);
void print_list_visual(Node *head);
void clear_list(Node **head);

int main(void) {
  int option = 0;
  Node *head = NULL;

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nExiting manager. Cleaning up memory...\n");
      clear_list(&head);
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    switch (option) {
    case 1:
      run_insert_front(&head);
      break;
    case 2:
      run_insert_back(&head);
      break;
    case 3:
      run_insert_pos(&head);
      break;
    case 4:
      run_delete_val(&head);
      break;
    case 5:
      run_search(head);
      break;
    case 6:
      run_reverse(&head);
      break;
    case 7:
      run_count(head);
      break;
    case 8:
      run_show(head);
      break;
    case 9:
      run_clear(&head);
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Singly Linked List Manager ===\n\n");
  printf("1. Insert at beginning\n2. Insert at end\n3. Insert at position\n"
         "4. Delete by value\n5. Search element\n6. Reverse list\n"
         "7. Count elements\n8. Show list\n9. Clear list\n10. Exit\n");
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

void run_insert_front(Node **head) {
  int value;
  printf("\nValue: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Status status = insert_front(head, value);
  if (status == SUCCESS) {
    printf("\n  - Node inserted at beginning.\n\n");
  } else {
    handle_error(status);
  }
}

void run_insert_back(Node **head) {
  int value;
  printf("\nValue: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Status status = insert_back(head, value);
  if (status == SUCCESS) {
    printf("\n  - Node inserted at end.\n\n");
  } else {
    handle_error(status);
  }
}

void run_insert_pos(Node **head) {
  int value, pos;
  printf("\nValue: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("Position (0 to N): ");
  if (read_integer(&pos) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Status status = insert_at(head, value, pos);
  if (status == SUCCESS) {
    printf("\n  - Node inserted at index %d.\n\n", pos);
  } else {
    handle_error(status);
  }
}

void run_delete_val(Node **head) {
  int value;
  printf("\nValue to delete: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Status status = delete_value(head, value);
  if (status == SUCCESS) {
    printf("\n  - Node with value %d deleted.\n\n", value);
  } else {
    handle_error(status);
  }
}

void run_search(Node *head) {
  int value;
  printf("\nSearch value: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  int index = search_node(head, value);
  if (index != -1) {
    printf("\n  - Found at index: %d\n\n", index);
  } else {
    handle_error(ERR_NOT_FOUND);
  }
}

void run_reverse(Node **head) {
  if (*head == NULL) {
    handle_error(ERR_LIST_EMPTY);
    return;
  }

  reverse_list(head);
  printf("\n  - List reversed successfully.\n\n");
}

void run_count(Node *head) {
  printf("\n  - Total elements: %d\n\n", count_nodes(head));
}

void run_show(Node *head) {
  print_list_visual(head);
  printf("  - Length: %d nodes\n\n", count_nodes(head));
}

void run_clear(Node **head) {
  clear_list(head);
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

Status insert_front(Node **head, int value) {
  Node *new_node = (Node *)malloc(sizeof(Node));
  if (!new_node) {
    return ERR_MEMORY_ALLOCATION;
  }

  new_node->value = value;
  new_node->next = *head;
  *head = new_node;

  return SUCCESS;
}

Status insert_back(Node **head, int value) {
  Node *new_node = (Node *)malloc(sizeof(Node));
  if (!new_node) {
    return ERR_MEMORY_ALLOCATION;
  }

  new_node->value = value;
  new_node->next = NULL;

  if (*head == NULL) {
    *head = new_node;
    return SUCCESS;
  }

  Node *temp = *head;
  while (temp->next != NULL) {
    temp = temp->next;
  }
  temp->next = new_node;

  return SUCCESS;
}

Status insert_at(Node **head, int value, int position) {
  if (position < 0) {
    return ERR_INVALID_POSITION;
  }

  if (position == 0) {
    return insert_front(head, value);
  }

  Node *temp = *head;
  for (int i = 0; i < position - 1; i++) {
    if (temp == NULL) {
      return ERR_INVALID_POSITION;
    }
    temp = temp->next;
  }

  if (temp == NULL) {
    return ERR_INVALID_POSITION;
  }

  Node *new_node = (Node *)malloc(sizeof(Node));
  if (!new_node) {
    return ERR_MEMORY_ALLOCATION;
  }

  new_node->value = value;
  new_node->next = temp->next;
  temp->next = new_node;

  return SUCCESS;
}

Status delete_value(Node **head, int value) {
  if (*head == NULL) {
    return ERR_LIST_EMPTY;
  }

  Node *temp = *head;
  Node *prev = NULL;

  // Case 1: Head holds the value
  if (temp != NULL && temp->value == value) {
    *head = temp->next;
    free(temp);
    return SUCCESS;
  }

  // Case 2: Search for value
  while (temp != NULL && temp->value != value) {
    prev = temp;
    temp = temp->next;
  }

  // If not found
  if (temp == NULL) {
    return ERR_NOT_FOUND;
  }

  // Unlink
  prev->next = temp->next;
  free(temp);

  return SUCCESS;
}

int search_node(Node *head, int value) {
  int index = 0;
  Node *temp = head;
  while (temp != NULL) {
    if (temp->value == value) {
      return index;
    }
    temp = temp->next;
    index++;
  }

  return -1;
}

Status reverse_list(Node **head) {
  Node *prev = NULL;
  Node *current = *head;
  Node *next = NULL;

  while (current != NULL) {
    next = current->next; // Store next
    current->next = prev; // Reverse pointer
    prev = current;       // Move prev one step
    current = next;       // Move current one step
  }

  *head = prev;
  return SUCCESS;
}

int count_nodes(Node *head) {
  int count = 0;
  while (head != NULL) {
    count++;
    head = head->next;
  }

  return count;
}

void print_list_visual(Node *head) {
  printf("\nCurrent list: ");
  Node *temp = head;
  while (temp != NULL) {
    printf("%d -> ", temp->value);
    temp = temp->next;
  }
  printf("NULL\n");
}

void clear_list(Node **head) {
  Node *current = *head;
  Node *next;

  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }
  *head = NULL;
}
