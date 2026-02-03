/*
 ===============================================================================
 Exercise: 01_dynamic_array.c
 Description: Dynamic Array Manager with auto-resizing
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Dynamic memory allocation (malloc/realloc)
 - Automatic resizing (growth factor: 2x)
 - Insert, Delete (by index), and Linear Search
 - Visual statistics (Capacity, Size, Load Factor)
 - Memory safety checks
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define INITIAL_CAPACITY 4
#define MIN_OPTION 1
#define MAX_OPTION 6

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_INVALID_INDEX,
  ERR_MEMORY_ALLOCATION,
  ERR_NOT_FOUND
} Status;

typedef struct {
  int *data;
  int size;
  int capacity;
} DynamicArray;

typedef struct {
  Status status;
  int value;
} Result;

void show_menu(void);
void handle_error(Status status);
void run_insert(DynamicArray *arr);
void run_delete(DynamicArray *arr);
void run_search(DynamicArray *arr);
void show_array_stats(const DynamicArray *arr);
void show_capacity_details(const DynamicArray *arr);

void clear_input_buffer(void);
Status read_integer(int *value);

Status resize_array(DynamicArray *arr);
void free_array(DynamicArray *arr);
Status init_array(DynamicArray *arr);
Status insert_item(DynamicArray *arr, int value);
Status delete_item(DynamicArray *arr, int index);
Result search_item(const DynamicArray *arr, int value);

int main(void) {
  int option = 0;
  DynamicArray arr;

  if (init_array(&arr) != SUCCESS) {
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
      printf("\nExiting program. Freeing memory...\n");
      free_array(&arr);
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    switch (option) {
    case 1:
      run_insert(&arr);
      break;
    case 2:
      run_delete(&arr);
      break;
    case 3:
      run_search(&arr);
      break;
    case 4:
      show_array_stats(&arr);
      break;
    case 5:
      show_capacity_details(&arr);
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Dynamic Array Manager ===\n\n");
  printf("1. Insert item\n2. Delete item (by index)\n3. Search item\n"
         "4. Show array\n5. View capacity details\n6. Exit\n");
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
  case ERR_INVALID_INDEX:
    printf("Error: Index out of bounds.\n\n");
    break;
  case ERR_MEMORY_ALLOCATION:
    printf("Error: System memory allocation failed.\n\n");
    break;
  case ERR_NOT_FOUND:
    printf("Result: Element not found.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_insert(DynamicArray *arr) {
  int value;
  printf("\nElement to insert: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Status status = insert_item(arr, value);
  if (status != SUCCESS) {
    handle_error(status);
  } else {
    printf("\n  - Inserted at index %d.\n\n", arr->size - 1);
  }
}

void run_delete(DynamicArray *arr) {
  int index;
  printf("\nEnter index to delete (0-%d): ", arr->size - 1);
  if (read_integer(&index) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Status status = delete_item(arr, index);
  if (status == SUCCESS) {
    printf("\n  - Element at index %d deleted.\n\n", index);
  } else {
    handle_error(status);
  }
}

void run_search(DynamicArray *arr) {
  int value;
  printf("\nSearch value: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Result res = search_item(arr, value);
  if (res.status == SUCCESS) {
    printf("\n  - Found at index %d\n\n", res.value);
  } else {
    handle_error(res.status);
  }
}

void show_array_stats(const DynamicArray *arr) {
  printf("\nCurrent array: [");
  for (int i = 0; i < arr->size; i++) {
    printf("%d", arr->data[i]);
    if (i < arr->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");

  double load_factor =
      (arr->capacity > 0) ? (double)arr->size / arr->capacity * 100.0 : 0.0;
  printf("  - Size: %d\n", arr->size);
  printf("  - Capacity: %d\n", arr->capacity);
  printf("  - Load Factor: %.1f%%\n\n", load_factor);
}

void show_capacity_details(const DynamicArray *arr) {
  printf("\nMemory Details:\n");
  printf("  - Total slots: %d\n", arr->capacity);
  printf("  - Used slots:  %d\n", arr->size);
  printf("  - Free slots:  %d\n", arr->capacity - arr->size);
  printf("  - Bytes used:  %lu bytes\n\n",
         (unsigned long)(arr->capacity * sizeof(int)));
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

Status init_array(DynamicArray *arr) {
  arr->data = (int *)malloc(INITIAL_CAPACITY * sizeof(int));
  if (arr->data == NULL) {
    return ERR_MEMORY_ALLOCATION;
  }

  arr->size = 0;
  arr->capacity = INITIAL_CAPACITY;

  return SUCCESS;
}

Status insert_item(DynamicArray *arr, int value) {
  if (arr->size == arr->capacity) {
    if (resize_array(arr) != SUCCESS) {
      return ERR_MEMORY_ALLOCATION;
    }
  }

  arr->data[arr->size++] = value;

  return SUCCESS;
}

Status delete_item(DynamicArray *arr, int index) {
  if (index < 0 || index >= arr->size) {
    return ERR_INVALID_INDEX;
  }

  for (int i = index; i < arr->size - 1; i++) {
    arr->data[i] = arr->data[i + 1];
  }

  arr->size--;

  return SUCCESS;
}

Result search_item(const DynamicArray *arr, int value) {
  Result res = {SUCCESS, -1};

  for (int i = 0; i < arr->size; i++) {
    if (arr->data[i] == value) {
      res.value = i;
      return res;
    }
  }

  res.status = ERR_NOT_FOUND;

  return res;
}

Status resize_array(DynamicArray *arr) {
  int new_capacity = arr->capacity * 2;
  int *new_data = (int *)realloc(arr->data, new_capacity * sizeof(int));

  if (new_data == NULL) {
    return ERR_MEMORY_ALLOCATION;
  }

  arr->data = new_data;
  arr->capacity = new_capacity;

  return SUCCESS;
}

void free_array(DynamicArray *arr) {
  if (arr->data != NULL) {
    free(arr->data);
    arr->data = NULL;
  }
  arr->size = 0;
  arr->capacity = 0;
}
