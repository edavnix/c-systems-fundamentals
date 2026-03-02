/*
 ===============================================================================
 Exercise: 02_memory_allocator.c
 Description: Dynamic Memory Manager Simulator (malloc, calloc, realloc, free)
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Wrapper functions for memory tracking
 - Block metadata storage (ID, size, address, status)
 - Simulation of memory leak detection
 - Real-time heap status visualization
 - Double-free protection
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BLOCKS 100
#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 7

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_MEMORY_ALLOCATION,
  ERR_BLOCK_NOT_FOUND,
  ERR_DOUBLE_FREE
} Status;

typedef struct {
  int id;
  void *ptr;
  size_t size;
  int is_freed;
  int is_initialized;
} MemBlock;

MemBlock memory_log[MAX_BLOCKS];
int block_count = 0;

void show_menu(void);
void handle_error(Status status);

void run_malloc(void);
void run_calloc(void);
void run_realloc(void);
void run_free(void);
void run_show_status(void);
void run_check_leaks(void);

void clear_input_buffer(void);
Status read_integer(int *value);
Status read_size(size_t *value);

void cleanup_all(void);

int main(void) {
  int option = 0;

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nExiting. Cleaning up remaining memory...\n");
      cleanup_all();
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    switch (option) {
    case 1:
      run_malloc();
      break;
    case 2:
      run_calloc();
      break;
    case 3:
      run_realloc();
      break;
    case 4:
      run_free();
      break;
    case 5:
      run_show_status();
      break;
    case 6:
      run_check_leaks();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Dynamic Memory Manager ===\n\n");
  printf("1. malloc()  - Allocate memory\n");
  printf("2. calloc()  - Allocate zero-initialized memory\n");
  printf("3. realloc() - Resize allocation\n");
  printf("4. free()    - Release memory\n");
  printf("5. Show Memory Status\n");
  printf("6. Detect Memory Leaks\n");
  printf("7. Exit\n");
  printf("Option: ");
}

void handle_error(Status status) {
  switch (status) {
  case ERR_INVALID_INPUT:
    printf("Error: Invalid input. Please enter a valid value.\n\n");
    break;
  case ERR_INVALID_OPTION:
    printf("Error: Invalid option selected.\n\n");
    break;
  case ERR_MEMORY_ALLOCATION:
    printf("Error: Memory allocation failed.\n\n");
    break;
  case ERR_BLOCK_NOT_FOUND:
    printf("Error: Block not found or already freed.\n\n");
    break;
  case ERR_DOUBLE_FREE:
    printf("Error: Double-free detected! Block was already released.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_malloc(void) {
  if (block_count >= MAX_BLOCKS) {
    printf("\n  - Tracking table full. Cannot register more blocks.\n\n");
    return;
  }

  size_t size;
  printf("\nSize to allocate (bytes): ");
  if (read_size(&size) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  void *ptr = malloc(size);
  if (!ptr) {
    handle_error(ERR_MEMORY_ALLOCATION);
    return;
  }

  block_count++;
  MemBlock *blk = &memory_log[block_count - 1];
  blk->id = block_count;
  blk->ptr = ptr;
  blk->size = size;
  blk->is_freed = FALSE;
  blk->is_initialized = FALSE;

  printf("\n  - malloc(%zu) successful.\n", size);
  printf("  - Address:  %p\n", ptr);
  printf("  - Block ID: #%d\n", block_count);
  printf("  - Content:  [uninitialized garbage]\n\n");
}

void run_calloc(void) {
  if (block_count >= MAX_BLOCKS) {
    printf("\n  - Tracking table full. Cannot register more blocks.\n\n");
    return;
  }

  size_t elements, elem_size;
  printf("\nNumber of elements: ");
  if (read_size(&elements) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("Size per element (bytes): ");
  if (read_size(&elem_size) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  void *ptr = calloc(elements, elem_size);
  if (!ptr) {
    handle_error(ERR_MEMORY_ALLOCATION);
    return;
  }

  size_t total_size = elements * elem_size;

  block_count++;
  MemBlock *blk = &memory_log[block_count - 1];
  blk->id = block_count;
  blk->ptr = ptr;
  blk->size = total_size;
  blk->is_freed = FALSE;
  blk->is_initialized = TRUE;

  printf("\n  - calloc(%zu, %zu) successful.\n", elements, elem_size);
  printf("  - Address:  %p\n", ptr);
  printf("  - Block ID: #%d\n", block_count);
  printf("  - Total:    %zu bytes\n", total_size);
  printf("  - Content:  [0, 0, 0, ..., 0] (zero-initialized)\n\n");
}

void run_realloc(void) {
  int id;
  printf("\nBlock ID to resize: #");
  if (read_integer(&id) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  int index = -1;
  for (int i = 0; i < block_count; i++) {
    if (memory_log[i].id == id && !memory_log[i].is_freed) {
      index = i;
      break;
    }
  }

  if (index == -1) {
    handle_error(ERR_BLOCK_NOT_FOUND);
    return;
  }

  size_t new_size;
  printf("New size (bytes): ");
  if (read_size(&new_size) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  void *old_ptr = memory_log[index].ptr;
  size_t old_size = memory_log[index].size;
  void *new_ptr = realloc(old_ptr, new_size);

  if (!new_ptr) {
    handle_error(ERR_MEMORY_ALLOCATION);
    return;
  }

  memory_log[index].ptr = new_ptr;
  memory_log[index].size = new_size;

  printf("\n  - realloc(ptr, %zu) successful.\n", new_size);
  printf("  - Old Address: %p (%zu bytes)\n", old_ptr, old_size);
  printf("  - New Address: %p (%zu bytes)\n", new_ptr, new_size);

  if (old_ptr == new_ptr) {
    printf("  - Location:    Same (expanded in place)\n");
  } else {
    printf("  - Location:    Moved (data copied to new address)\n");
  }
  printf("  - Content:     Preserved\n\n");
}

void run_free(void) {
  int id;
  printf("\nBlock ID to free: #");
  if (read_integer(&id) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  for (int i = 0; i < block_count; i++) {
    if (memory_log[i].id == id) {
      if (memory_log[i].is_freed) {
        handle_error(ERR_DOUBLE_FREE);
        return;
      }

      free(memory_log[i].ptr);
      memory_log[i].is_freed = TRUE;
      memory_log[i].ptr = NULL;

      printf("\n  - free() successful.\n");
      printf("  - Block #%d released (%zu bytes returned to heap).\n\n", id,
             memory_log[i].size);
      return;
    }
  }

  handle_error(ERR_BLOCK_NOT_FOUND);
}

void run_show_status(void) {
  int active = 0;
  int freed = 0;
  size_t total_active = 0;
  size_t total_freed = 0;

  for (int i = 0; i < block_count; i++) {
    if (!memory_log[i].is_freed) {
      active++;
      total_active += memory_log[i].size;
    } else {
      freed++;
      total_freed += memory_log[i].size;
    }
  }

  printf("\n=== Heap Memory Status ===\n\n");
  printf("  Active Blocks:  %d (%zu bytes)\n", active, total_active);
  printf("  Freed Blocks:   %d (%zu bytes returned)\n", freed, total_freed);
  printf("  Total Tracked:  %d blocks\n\n", block_count);

  if (active == 0) {
    printf("  (No active allocations)\n\n");
    return;
  }

  printf("  %-5s | %-18s | %-10s | %-8s | %s\n", "ID", "Address", "Size",
         "Status", "Init");
  printf("  -------|--------------------|-----------|----------|------\n");

  for (int i = 0; i < block_count; i++) {
    if (!memory_log[i].is_freed) {
      printf("  #%-4d | %-18p | %-7zu B  | ACTIVE   | %s\n", memory_log[i].id,
             memory_log[i].ptr, memory_log[i].size,
             memory_log[i].is_initialized ? "zeroed" : "garbage");
    }
  }
  printf("\n");
}

void run_check_leaks(void) {
  printf("\n=== Memory Leak Analysis ===\n\n");

  int leaks = 0;
  size_t leaked_bytes = 0;

  for (int i = 0; i < block_count; i++) {
    if (!memory_log[i].is_freed) {
      leaks++;
      leaked_bytes += memory_log[i].size;
      printf("  ⚠ LEAK: Block #%d (%zu bytes at %p) never freed.\n",
             memory_log[i].id, memory_log[i].size, memory_log[i].ptr);
    }
  }

  if (leaks == 0) {
    printf("  ✓ No leaks detected. All allocations properly freed.\n\n");
  } else {
    printf("\n  - Total leaks:  %d block(s)\n", leaks);
    printf("  - Leaked bytes: %zu\n", leaked_bytes);
    printf("  - Action: Call free() for each malloc/calloc allocation.\n\n");
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

Status read_size(size_t *value) {
  if (scanf("%zu", value) != 1) {
    clear_input_buffer();
    return ERR_INVALID_INPUT;
  }
  clear_input_buffer();
  return SUCCESS;
}

void cleanup_all(void) {
  int cleaned = 0;
  for (int i = 0; i < block_count; i++) {
    if (!memory_log[i].is_freed) {
      free(memory_log[i].ptr);
      memory_log[i].is_freed = TRUE;
      memory_log[i].ptr = NULL;
      cleaned++;
    }
  }
  printf("  - Cleaned up %d remaining block(s).\n", cleaned);
}
