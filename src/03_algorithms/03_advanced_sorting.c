/*
 ===============================================================================
 Exercise: 03_advanced_sorting.c
 Description: Advanced Sorting Algorithms (Merge Sort vs Quick Sort)
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Implementation of O(n log n) algorithms: Merge Sort & Quick Sort
 - Performance benchmarking (Time & Comparisons)
 - Dynamic comparison against O(n²) Bubble Sort (Estimated)
 - Handling of large arrays (heap allocation)
 - Dynamic memory management with proper cleanup
 ===============================================================================
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 3

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_MEMORY_ALLOCATION
} Status;

typedef struct {
  unsigned long long comparisons;
  double time_taken;
} SortStats;

void show_menu(void);
void handle_error(Status status);
void run_benchmark(void);
void run_algorithm_info(void);

void clear_input_buffer(void);
Status read_integer(int *value);

Status generate_random_array(int **arr, int size);
void run_merge_sort(int *arr, int size, SortStats *stats);
void run_quick_sort(int *arr, int size, SortStats *stats);
void merge_sort_recursive(int *arr, int l, int r, int *temp,
                          unsigned long long *comps);
void merge(int *arr, int l, int m, int r, int *temp, unsigned long long *comps);
void quick_sort_recursive(int *arr, int low, int high,
                          unsigned long long *comps);
int partition(int *arr, int low, int high, unsigned long long *comps);
void show_final_comparison(int size, SortStats merge_stats,
                           SortStats quick_stats);
void print_array_preview(const int *arr, int size);
void copy_array(const int *src, int *dest, int size);
void swap(int *a, int *b);

int main(void) {
  int option = 0;
  srand(time(NULL));

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

    switch (option) {
    case 1:
      run_benchmark();
      break;
    case 2:
      run_algorithm_info();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Algoritmos de Ordenamiento Avanzados ===\n\n");
  printf("1. Ejecutar Benchmark (Merge Sort vs Quick Sort)\n"
         "2. Información de Algoritmos\n"
         "3. Salir\n");
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
  case ERR_MEMORY_ALLOCATION:
    printf("Error: Memoria insuficiente.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_benchmark(void) {
  int size = 0;
  int *master_arr = NULL;
  int *work_arr = NULL;
  SortStats merge_stats = {0, 0.0};
  SortStats quick_stats = {0, 0.0};

  printf("\nIngrese tamaño del array (Recomendado 1000+): ");
  if (read_integer(&size) != SUCCESS || size < 2) {
    printf("Tamaño inválido. Usando defecto (10000).\n");
    size = 10000;
  }

  printf("Generando array aleatorio de %d elementos...\n", size);
  if (generate_random_array(&master_arr, size) != SUCCESS) {
    handle_error(ERR_MEMORY_ALLOCATION);
    return;
  }

  work_arr = (int *)malloc(size * sizeof(int));
  if (work_arr == NULL) {
    free(master_arr);
    handle_error(ERR_MEMORY_ALLOCATION);
    return;
  }

  printf("Array generado: ");
  print_array_preview(master_arr, size);

  printf("\n=== Merge Sort ===\n");
  copy_array(master_arr, work_arr, size);
  run_merge_sort(work_arr, size, &merge_stats);

  printf("\n=== Quick Sort ===\n");
  copy_array(master_arr, work_arr, size);
  run_quick_sort(work_arr, size, &quick_stats);

  show_final_comparison(size, merge_stats, quick_stats);

  free(master_arr);
  free(work_arr);
}

void run_algorithm_info(void) {
  printf("\n=== Información de Algoritmos ===\n\n");
  printf("Merge Sort:\n");
  printf("  - Complejidad: O(n log n) en todos los casos\n");
  printf("  - Memoria adicional: O(n)\n");
  printf("  - Estable: Sí\n\n");
  printf("Quick Sort:\n");
  printf("  - Complejidad: O(n log n) promedio, O(n²) peor caso\n");
  printf("  - Memoria adicional: O(log n) (stack de recursión)\n");
  printf("  - Estable: No\n\n");
  printf("Recomendación:\n");
  printf("  - Merge Sort: cuando se necesita estabilidad garantizada\n");
  printf("  - Quick Sort: mejor rendimiento en promedio para datos "
         "aleatorios\n\n");
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

Status generate_random_array(int **arr, int size) {
  *arr = (int *)malloc(size * sizeof(int));
  if (*arr == NULL) {
    return ERR_MEMORY_ALLOCATION;
  }

  for (int i = 0; i < size; i++) {
    (*arr)[i] = rand() % 10000;
  }
  return SUCCESS;
}

void run_merge_sort(int *arr, int size, SortStats *stats) {
  printf("Ejecutando...\n");

  int *temp = (int *)malloc(size * sizeof(int));
  if (temp == NULL) {
    handle_error(ERR_MEMORY_ALLOCATION);
    return;
  }

  clock_t start = clock();
  merge_sort_recursive(arr, 0, size - 1, temp, &stats->comparisons);
  clock_t end = clock();

  stats->time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
  free(temp);

  printf("  - Tiempo:        %.6f segundos\n", stats->time_taken);
  printf("  - Comparaciones: %llu\n", stats->comparisons);
  printf("  - Complejidad:   O(n log n)\n");
  printf("  - Memoria extra: O(n)\n");
}

void merge_sort_recursive(int *arr, int l, int r, int *temp,
                          unsigned long long *comps) {
  if (l < r) {
    int m = l + (r - l) / 2;
    merge_sort_recursive(arr, l, m, temp, comps);
    merge_sort_recursive(arr, m + 1, r, temp, comps);
    merge(arr, l, m, r, temp, comps);
  }
}

void merge(int *arr, int l, int m, int r, int *temp,
           unsigned long long *comps) {
  int i = l;
  int j = m + 1;
  int k = l;

  while (i <= m && j <= r) {
    (*comps)++;
    if (arr[i] <= arr[j]) {
      temp[k++] = arr[i++];
    } else {
      temp[k++] = arr[j++];
    }
  }

  while (i <= m) {
    temp[k++] = arr[i++];
  }
  while (j <= r) {
    temp[k++] = arr[j++];
  }

  for (i = l; i <= r; i++) {
    arr[i] = temp[i];
  }
}

void run_quick_sort(int *arr, int size, SortStats *stats) {
  printf("Ejecutando...\n");

  clock_t start = clock();
  quick_sort_recursive(arr, 0, size - 1, &stats->comparisons);
  clock_t end = clock();

  stats->time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

  printf("  - Tiempo:        %.6f segundos\n", stats->time_taken);
  printf("  - Comparaciones: %llu\n", stats->comparisons);
  printf("  - Complejidad:   O(n log n) promedio\n");
  printf("  - Memoria extra: O(log n) (Stack)\n");
}

void quick_sort_recursive(int *arr, int low, int high,
                          unsigned long long *comps) {
  if (low < high) {
    int pi = partition(arr, low, high, comps);
    quick_sort_recursive(arr, low, pi - 1, comps);
    quick_sort_recursive(arr, pi + 1, high, comps);
  }
}

int partition(int *arr, int low, int high, unsigned long long *comps) {
  int pivot = arr[high];
  int i = (low - 1);

  for (int j = low; j <= high - 1; j++) {
    (*comps)++;
    if (arr[j] < pivot) {
      i++;
      swap(&arr[i], &arr[j]);
    }
  }

  swap(&arr[i + 1], &arr[high]);
  return (i + 1);
}

void show_final_comparison(int size, SortStats merge_stats,
                           SortStats quick_stats) {
  double n = (double)size;
  double log_n = log2(n);
  double ratio = (n * n) / (n * log_n);
  double base_time =
      (quick_stats.time_taken > 0) ? quick_stats.time_taken : 0.000001;
  double estimated_bubble_time = base_time * ratio * 0.5;

  printf("\n=== Comparación con Bubble Sort (estimado) ===\n\n");
  printf("  - Bubble Sort tomaría: ~%.4f segundos\n", estimated_bubble_time);

  if (quick_stats.time_taken > 0) {
    printf("  - Quick Sort es %.0fx más rápido que Bubble Sort\n",
           estimated_bubble_time / quick_stats.time_taken);
  }
  if (merge_stats.time_taken > 0) {
    printf("  - Merge Sort es %.0fx más rápido que Bubble Sort\n",
           estimated_bubble_time / merge_stats.time_taken);
  }

  printf("\n  - Recomendación para arrays grandes (>1000 elementos):\n");
  printf("    + Quick Sort: más rápido en promedio\n");
  printf("    + Merge Sort: estable y predecible\n\n");
}

void print_array_preview(const int *arr, int size) {
  printf("[");
  if (size <= 10) {
    for (int i = 0; i < size; i++) {
      printf("%d%s", arr[i], (i < size - 1) ? ", " : "");
    }
  } else {
    printf("%d, %d, %d, ..., %d", arr[0], arr[1], arr[2], arr[size - 1]);
  }
  printf("]\n");
}

void copy_array(const int *src, int *dest, int size) {
  for (int i = 0; i < size; i++) {
    dest[i] = src[i];
  }
}

void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}
