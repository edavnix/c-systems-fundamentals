/*
 ===============================================================================
 Exercise: 02_sorting_visualizer.c
 Description: Sorting Visualizer (Bubble, Selection, Insertion)
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Algorithm selection menu (Bubble, Selection, Insertion Sort)
 - Step-by-step visualization of swaps/shifts
 - Statistical tracking (Comparisons, Swaps, Time)
 - Input validation and dynamic array handling
 - Interactive menu for multiple runs with same data
 ===============================================================================
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define MAX_SIZE 100
#define MIN_OPTION 1
#define MAX_OPTION 5

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION
} Status;

typedef struct {
  unsigned long comparisons;
  unsigned long swaps;
  double time_taken;
} SortStats;

typedef enum {
  ALGO_BUBBLE = 1,
  ALGO_SELECTION = 2,
  ALGO_INSERTION = 3
} AlgorithmType;

void show_menu(void);
void handle_error(Status status);
void run_bubble_sort(int *arr, int size);
void run_selection_sort(int *arr, int size);
void run_insertion_sort(int *arr, int size);
void run_enter_array(int *arr, int *size);

void clear_input_buffer(void);
Status read_integer(int *value);

void print_stats(SortStats stats, const char *complexity);
void enter_custom_array(int *arr, int *size);
void copy_array(const int *src, int *dest, int size);
void print_array(const int *arr, int size);
void print_array_inline(const int *arr, int size);
void swap(int *a, int *b);

int main(void) {
  int raw_arr[MAX_SIZE];
  int work_arr[MAX_SIZE];
  int size = 0;
  int option = 0;

  srand(time(NULL));

  printf("\n=== Configuración de Datos ===\n");
  enter_custom_array(raw_arr, &size);

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nSaliendo. Hasta luego!\n");
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    if (option == 4) {
      enter_custom_array(raw_arr, &size);
      continue;
    }

    copy_array(raw_arr, work_arr, size);

    printf("\n=== Visualizador de Ordenamiento ===\n");
    printf("Array original: ");
    print_array(work_arr, size);

    switch (option) {
    case ALGO_BUBBLE:
      run_bubble_sort(work_arr, size);
      break;
    case ALGO_SELECTION:
      run_selection_sort(work_arr, size);
      break;
    case ALGO_INSERTION:
      run_insertion_sort(work_arr, size);
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Seleccione Algoritmo ===\n\n");
  printf("1. Bubble Sort\n2. Selection Sort\n3. Insertion Sort\n"
         "4. Ingresar nuevo array\n5. Salir\n");
  printf("Opción: ");
}

void handle_error(Status status) {
  switch (status) {
  case ERR_INVALID_INPUT:
    printf("Error: Entrada inválida. Por favor ingrese un número.\n\n");
    break;
  case ERR_INVALID_OPTION:
    printf("Error: Opción inválida seleccionada.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_bubble_sort(int *arr, int size) {
  SortStats stats = {0, 0, 0.0};
  clock_t start = clock();
  bool swapped;

  printf("Algoritmo: Bubble Sort\n");

  for (int i = 0; i < size - 1; i++) {
    swapped = FALSE;
    printf("\nPasada %d:\n", i + 1);

    for (int j = 0; j < size - i - 1; j++) {
      stats.comparisons++;
      if (arr[j] > arr[j + 1]) {
        int val_a = arr[j];
        int val_b = arr[j + 1];

        swap(&arr[j], &arr[j + 1]);
        stats.swaps++;
        swapped = TRUE;

        print_array_inline(arr, size);
        printf(" - Swap: %d<->%d\n", val_a, val_b);
      }
    }

    if (!swapped) {
      printf("(Array ya ordenado, terminando prematuramente)\n");
      break;
    }
  }

  clock_t end = clock();
  stats.time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
  print_stats(stats, "O(n²)");

  printf("Array ordenado: ");
  print_array(arr, size);
}

void run_selection_sort(int *arr, int size) {
  SortStats stats = {0, 0, 0.0};
  clock_t start = clock();

  printf("Algoritmo: Selection Sort\n");

  for (int i = 0; i < size - 1; i++) {
    int min_idx = i;
    printf("\nPasada %d (Buscando mínimo desde índice %d):\n", i + 1, i);

    for (int j = i + 1; j < size; j++) {
      stats.comparisons++;
      if (arr[j] < arr[min_idx]) {
        min_idx = j;
      }
    }

    if (min_idx != i) {
      int val_a = arr[i];
      int val_b = arr[min_idx];

      swap(&arr[min_idx], &arr[i]);
      stats.swaps++;

      print_array_inline(arr, size);
      printf(" - Swap: %d (actual) <-> %d (mínimo)\n", val_a, val_b);
    } else {
      print_array_inline(arr, size);
      printf(" - %d ya es el mínimo, sin cambios.\n", arr[i]);
    }
  }

  clock_t end = clock();
  stats.time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
  print_stats(stats, "O(n²)");

  printf("Array ordenado: ");
  print_array(arr, size);
}

void run_insertion_sort(int *arr, int size) {
  SortStats stats = {0, 0, 0.0};
  clock_t start = clock();

  printf("Algoritmo: Insertion Sort\n");

  for (int i = 1; i < size; i++) {
    int key = arr[i];
    int j = i - 1;

    printf("\nPasada %d (Insertando %d):\n", i, key);

    while (j >= 0 && arr[j] > key) {
      stats.comparisons++;
      arr[j + 1] = arr[j];
      stats.swaps++;
      j = j - 1;
    }

    if (j >= 0) {
      stats.comparisons++;
    }

    arr[j + 1] = key;

    print_array_inline(arr, size);
    printf(" - %d insertado en posición %d\n", key, j + 1);
  }

  clock_t end = clock();
  stats.time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
  print_stats(stats, "O(n²)");

  printf("Array ordenado: ");
  print_array(arr, size);
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

void print_stats(SortStats stats, const char *complexity) {
  printf("\nEstadísticas:\n");
  printf("  - Comparaciones: %lu\n", stats.comparisons);
  printf("  - Intercambios:  %lu\n", stats.swaps);
  printf("  - Tiempo:        %.6f segundos\n", stats.time_taken);
  printf("  - Complejidad:   %s\n\n", complexity);
}

void enter_custom_array(int *arr, int *size) {
  printf("\nIngrese tamaño del array (Máx %d): ", MAX_SIZE);
  if (read_integer(size) != SUCCESS || *size < 2 || *size > MAX_SIZE) {
    printf("Tamaño inválido. Usando defecto (5).\n");
    *size = 5;
    int def[] = {64, 25, 12, 22, 11};
    copy_array(def, arr, 5);
    return;
  }

  printf("Ingrese %d enteros separados por espacio:\n", *size);
  for (int i = 0; i < *size; i++) {
    if (scanf("%d", &arr[i]) != 1) {
      clear_input_buffer();
      arr[i] = 0;
    }
  }
  clear_input_buffer();
}

void copy_array(const int *src, int *dest, int size) {
  for (int i = 0; i < size; i++) {
    dest[i] = src[i];
  }
}

void print_array(const int *arr, int size) {
  printf("[");
  for (int i = 0; i < size; i++) {
    printf("%d%s", arr[i], (i < size - 1) ? ", " : "");
  }
  printf("]\n");
}

void print_array_inline(const int *arr, int size) {
  printf("[");
  for (int i = 0; i < size; i++) {
    printf("%d%s", arr[i], (i < size - 1) ? ", " : "");
  }
  printf("]");
}

void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}
