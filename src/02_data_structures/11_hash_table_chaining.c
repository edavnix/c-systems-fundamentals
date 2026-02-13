/*
 ===============================================================================
 Exercise: 11_hash_table_chaining.c
 Description: Hash Table implementation using chaining (linked lists)
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Dynamic Buckets (Array of Linked Lists)
 - Hash Function: Sum of ASCII values % Capacity
 - Collision Resolution: Chaining (Append to tail)
 - Statistics: Load Factor, Collision Rate, Longest Chain
 - Rehash capability (Doubling capacity)
 - Dynamic memory management with proper cleanup
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define INITIAL_CAPACITY 10
#define MAX_KEY_LEN 64
#define MIN_OPTION 1
#define MAX_OPTION 7

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_MEMORY_ALLOCATION,
  ERR_KEY_NOT_FOUND
} Status;

typedef struct Node {
  char key[MAX_KEY_LEN];
  int value;
  struct Node *next;
} Node;

typedef struct {
  Node **buckets;
  int capacity;
  int count;
} HashTable;

void show_menu(int capacity);
void handle_error(Status status);
void run_insert(HashTable *ht);
void run_search(HashTable *ht);
void run_delete(HashTable *ht);
void run_show(const HashTable *ht);
void run_stats(const HashTable *ht);
void run_rehash(HashTable *ht);

void clear_input_buffer(void);
Status read_integer(int *value);
void read_string(char *buffer, int max_len);

Status init_table(HashTable *ht, int capacity);
int hash_function(const char *key, int capacity);
Status insert(HashTable *ht, const char *key, int value);
Status search(const HashTable *ht, const char *key, int *value);
Status delete_key(HashTable *ht, const char *key);
Status rehash_table(HashTable *ht);
void clear_table(HashTable *ht);

int main(void) {
  int option = 0;
  HashTable ht;

  if (init_table(&ht, INITIAL_CAPACITY) != SUCCESS) {
    printf("Fatal Error: Memory allocation failed.\n");
    return 1;
  }

  while (TRUE) {
    show_menu(ht.capacity);

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nExiting. Cleaning up memory...\n");
      clear_table(&ht);
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    switch (option) {
    case 1:
      run_insert(&ht);
      break;
    case 2:
      run_search(&ht);
      break;
    case 3:
      run_delete(&ht);
      break;
    case 4:
      run_show(&ht);
      break;
    case 5:
      run_stats(&ht);
      break;
    case 6:
      run_rehash(&ht);
      break;
    }
  }

  return 0;
}

void show_menu(int capacity) {
  printf("=== Hash Table (Chaining) ===\n");
  printf("Capacity: %d buckets\n", capacity);
  printf("Hash Func: (Sum ASCII) %% %d\n\n", capacity);
  printf("1. Insert key-value pair\n2. Search by key\n3. Delete by key\n"
         "4. Show Table (Visual)\n5. Statistics\n"
         "6. Rehash (Double Capacity)\n7. Exit\n");
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
    printf("Error: System memory allocation failed.\n\n");
    break;
  case ERR_KEY_NOT_FOUND:
    printf("Error: Key not found.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_insert(HashTable *ht) {
  char key[MAX_KEY_LEN];
  int value;

  printf("\nKey: ");
  read_string(key, MAX_KEY_LEN);
  printf("Value: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  int idx = hash_function(key, ht->capacity);
  printf("\n  - Hash(%s) = %d\n", key, idx);

  if (ht->buckets[idx] != NULL) {
    printf("  - ⚠ Collision detected at bucket %d\n", idx);
    printf("  - Inserted into chain.\n\n");
  } else {
    printf("  - Inserted at bucket %d\n\n", idx);
  }

  Status status = insert(ht, key, value);
  if (status != SUCCESS) {
    handle_error(status);
  }
}

void run_search(HashTable *ht) {
  char key[MAX_KEY_LEN];
  int value;

  printf("\nKey to search: ");
  read_string(key, MAX_KEY_LEN);

  Status status = search(ht, key, &value);
  if (status == SUCCESS) {
    int idx = hash_function(key, ht->capacity);
    printf("\n  - Found: [%s : %d] in bucket %d\n\n", key, value, idx);
  } else {
    handle_error(status);
  }
}

void run_delete(HashTable *ht) {
  char key[MAX_KEY_LEN];
  printf("\nKey to delete: ");
  read_string(key, MAX_KEY_LEN);

  Status status = delete_key(ht, key);
  if (status == SUCCESS) {
    printf("\n  - Key '%s' deleted successfully.\n\n", key);
  } else {
    handle_error(status);
  }
}

void run_show(const HashTable *ht) {
  printf("\nHash Table Content:\n");
  for (int i = 0; i < ht->capacity; i++) {
    printf("Bucket %d: ", i);
    Node *current = ht->buckets[i];

    if (current == NULL) {
      printf("empty\n");
    } else {
      while (current != NULL) {
        printf("[%s:%d]", current->key, current->value);
        if (current->next != NULL) {
          printf(" -> ");
        } else {
          printf(" -> NULL");
        }
        current = current->next;
      }
      printf("\n");
    }
  }
  printf("\n");
}

void run_stats(const HashTable *ht) {
  int used_buckets = 0;
  int collisions = 0;
  int max_chain = 0;

  for (int i = 0; i < ht->capacity; i++) {
    if (ht->buckets[i] != NULL) {
      used_buckets++;
      int chain_len = 0;
      Node *curr = ht->buckets[i];
      while (curr) {
        chain_len++;
        curr = curr->next;
      }
      if (chain_len > 1) {
        collisions += (chain_len - 1);
      }
      if (chain_len > max_chain) {
        max_chain = chain_len;
      }
    }
  }

  double load_factor = (double)ht->count / ht->capacity;

  printf("\nStatistics:\n");
  printf("  - Total Elements: %d\n", ht->count);
  printf("  - Used Buckets: %d/%d (%.0f%%)\n", used_buckets, ht->capacity,
         (double)used_buckets / ht->capacity * 100);
  printf("  - Load Factor: %.2f\n", load_factor);
  printf("  - Collisions (Nodes beyond first): %d\n", collisions);
  printf("  - Longest Chain: %d\n", max_chain);
  printf("  - Efficiency: ");
  if (load_factor < 0.75 && max_chain < 3) {
    printf("GOOD ✓\n\n");
  } else {
    printf("DEGRADING (Consider Rehash) ⚠\n\n");
  }
}

void run_rehash(HashTable *ht) {
  printf("\n  - Rehashing table...\n");
  Status status = rehash_table(ht);
  if (status == SUCCESS) {
    printf("  - Rehash complete. New capacity: %d\n\n", ht->capacity);
  } else {
    handle_error(status);
  }
}

void clear_input_buffer(void) {
  int c;
  while ((c = getchar()) != '\n' && c != EOF) {
    ;
  }
}

void read_string(char *buffer, int max_len) {
  if (fgets(buffer, max_len, stdin) != NULL) {
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
    }
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

Status init_table(HashTable *ht, int capacity) {
  ht->capacity = capacity;
  ht->count = 0;
  ht->buckets = (Node **)calloc(capacity, sizeof(Node *));

  if (ht->buckets == NULL) {
    return ERR_MEMORY_ALLOCATION;
  }

  return SUCCESS;
}

int hash_function(const char *key, int capacity) {
  unsigned int sum = 0;
  for (int i = 0; key[i] != '\0'; i++) {
    sum += (unsigned int)key[i];
  }

  return sum % capacity;
}

Status insert(HashTable *ht, const char *key, int value) {
  int idx = hash_function(key, ht->capacity);
  Node *current = ht->buckets[idx];

  while (current != NULL) {
    if (strcmp(current->key, key) == 0) {
      current->value = value;
      return SUCCESS;
    }
    current = current->next;
  }

  Node *new_node = (Node *)malloc(sizeof(Node));
  if (!new_node) {
    return ERR_MEMORY_ALLOCATION;
  }

  strncpy(new_node->key, key, MAX_KEY_LEN - 1);
  new_node->key[MAX_KEY_LEN - 1] = '\0';
  new_node->value = value;
  new_node->next = NULL;

  if (ht->buckets[idx] == NULL) {
    ht->buckets[idx] = new_node;
  } else {
    Node *tail = ht->buckets[idx];
    while (tail->next != NULL) {
      tail = tail->next;
    }
    tail->next = new_node;
  }

  ht->count++;

  return SUCCESS;
}

Status search(const HashTable *ht, const char *key, int *value) {
  int idx = hash_function(key, ht->capacity);
  Node *current = ht->buckets[idx];

  while (current != NULL) {
    if (strcmp(current->key, key) == 0) {
      *value = current->value;
      return SUCCESS;
    }
    current = current->next;
  }

  return ERR_KEY_NOT_FOUND;
}

Status delete_key(HashTable *ht, const char *key) {
  int idx = hash_function(key, ht->capacity);
  Node *current = ht->buckets[idx];
  Node *prev = NULL;

  while (current != NULL) {
    if (strcmp(current->key, key) == 0) {
      if (prev == NULL) {
        ht->buckets[idx] = current->next;
      } else {
        prev->next = current->next;
      }
      free(current);
      ht->count--;
      return SUCCESS;
    }
    prev = current;
    current = current->next;
  }

  return ERR_KEY_NOT_FOUND;
}

Status rehash_table(HashTable *ht) {
  int old_capacity = ht->capacity;
  Node **old_buckets = ht->buckets;

  int new_capacity = old_capacity * 2;
  Node **new_buckets = (Node **)calloc(new_capacity, sizeof(Node *));
  if (new_buckets == NULL) {
    return ERR_MEMORY_ALLOCATION;
  }

  ht->capacity = new_capacity;
  ht->buckets = new_buckets;
  ht->count = 0;

  for (int i = 0; i < old_capacity; i++) {
    Node *curr = old_buckets[i];
    while (curr != NULL) {
      Node *next = curr->next;
      insert(ht, curr->key, curr->value);
      free(curr);
      curr = next;
    }
  }

  free(old_buckets);

  return SUCCESS;
}

void clear_table(HashTable *ht) {
  for (int i = 0; i < ht->capacity; i++) {
    Node *current = ht->buckets[i];
    while (current != NULL) {
      Node *temp = current;
      current = current->next;
      free(temp);
    }
  }
  free(ht->buckets);
  ht->buckets = NULL;
  ht->count = 0;
}
