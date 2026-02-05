/*
 ===============================================================================
 Exercise: 03_matrix_calculator.c
 Description: Matrix calculator with dynamic memory management
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Dynamic storage of matrices (Starts at 2, Grows to Max 10)
 - Matrix creation with ID (A-Z)
 - Matrix Addition, Multiplication, Transposition
 - Determinant calculation (Recursive)
 - Dimension validation and error handling
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define INITIAL_CAPACITY 2
#define MAX_CAPACITY 10
#define MIN_OPTION 1
#define MAX_OPTION 7

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_MEMORY_ALLOCATION,
  ERR_SYSTEM_FULL,
  ERR_MATRIX_NOT_FOUND,
  ERR_DUPLICATE_ID,
  ERR_INCOMPATIBLE_DIM,
  ERR_NOT_SQUARE
} Status;

typedef struct {
  char id;
  int rows;
  int cols;
  double **data;
} Matrix;

typedef struct {
  Matrix *list;
  int count;
  int capacity;
} MatrixSystem;

typedef struct {
  Status status;
  double value;
} Result;

void show_menu(void);
void handle_error(Status status);
void print_matrix(const Matrix *mat);
void list_available_matrices(const MatrixSystem *sys);

void run_create_matrix(MatrixSystem *sys);
void run_add_matrices(MatrixSystem *sys);
void run_multiply_matrices(MatrixSystem *sys);
void run_transpose_matrix(MatrixSystem *sys);
void run_determinant(MatrixSystem *sys);
void run_show_matrix(MatrixSystem *sys);

void clear_input_buffer(void);
Status read_integer(int *value);
Status read_double(double *value);
Status read_char(char *value);

Status init_system(MatrixSystem *sys);
Status resize_system(MatrixSystem *sys);
void free_system(MatrixSystem *sys);
Status create_matrix(MatrixSystem *sys, char id, int rows, int cols);
void free_matrix_data(Matrix *mat);
int find_matrix_index(const MatrixSystem *sys, char id);
Status get_matrix_by_id(const MatrixSystem *sys, char id, Matrix **mat);
Status add_matrices(const Matrix *a, const Matrix *b, Matrix *result);
Status multiply_matrices(const Matrix *a, const Matrix *b, Matrix *result);
Status transpose_matrix(const Matrix *src, Matrix *dest);
Result calculate_determinant(double **data, int n);
void get_cofactor(double **data, double **temp, int p, int q, int n);

int main(void) {
  int option = 0;
  MatrixSystem sys;

  if (init_system(&sys) != SUCCESS) {
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
      printf("\nExiting calculator. Freeing memory...\n");
      free_system(&sys);
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    switch (option) {
    case 1:
      run_create_matrix(&sys);
      break;
    case 2:
      run_add_matrices(&sys);
      break;
    case 3:
      run_multiply_matrices(&sys);
      break;
    case 4:
      run_transpose_matrix(&sys);
      break;
    case 5:
      run_determinant(&sys);
      break;
    case 6:
      run_show_matrix(&sys);
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Matrix Calculator ===\n\n");
  printf("1. Create matrix\n2. Add matrices\n3. Multiply matrices\n"
         "4. Transpose matrix\n5. Calculate determinant\n6. Show matrix\n"
         "7. Exit\n");
  printf("Option: ");
}

void handle_error(Status status) {
  switch (status) {
  case ERR_INVALID_INPUT:
    printf("Error: Invalid input. Please try again.\n\n");
    break;
  case ERR_INVALID_OPTION:
    printf("Error: Invalid option selected.\n\n");
    break;
  case ERR_MEMORY_ALLOCATION:
    printf("Error: Memory allocation failed.\n\n");
    break;
  case ERR_SYSTEM_FULL:
    printf("Error: Matrix storage full (Max %d).\n\n", MAX_CAPACITY);
    break;
  case ERR_MATRIX_NOT_FOUND:
    printf("Error: Matrix ID not found.\n\n");
    break;
  case ERR_DUPLICATE_ID:
    printf("Error: Matrix ID already exists.\n\n");
    break;
  case ERR_INCOMPATIBLE_DIM:
    printf("Error: Incompatible dimensions for operation.\n\n");
    break;
  case ERR_NOT_SQUARE:
    printf("Error: Matrix must be square for determinant.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void print_matrix(const Matrix *mat) {
  for (int i = 0; i < mat->rows; i++) {
    printf("[ ");
    for (int j = 0; j < mat->cols; j++) {
      printf("%6.2f ", mat->data[i][j]);
    }
    printf("]\n");
  }
}

void list_available_matrices(const MatrixSystem *sys) {
  if (sys->count == 0) {
    printf("\n(No matrices created yet)\n");
    return;
  }

  printf("\nAvailable Matrices:\n");
  for (int i = 0; i < sys->count; i++) {
    printf("  - ID: %c [%dx%d]\n", sys->list[i].id, sys->list[i].rows,
           sys->list[i].cols);
  }
}

void run_create_matrix(MatrixSystem *sys) {
  char id;
  int rows, cols;

  if (sys->count >= sys->capacity) {
    if (resize_system(sys) != SUCCESS) {
      handle_error(ERR_SYSTEM_FULL);
      return;
    }
  }

  printf("\nMatrix ID (A-Z): ");
  read_char(&id);

  if (find_matrix_index(sys, id) != -1) {
    handle_error(ERR_DUPLICATE_ID);
    return;
  }

  printf("Rows: ");
  if (read_integer(&rows) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }
  printf("Columns: ");
  if (read_integer(&cols) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  if (rows <= 0 || cols <= 0) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Status status = create_matrix(sys, id, rows, cols);
  if (status != SUCCESS) {
    handle_error(status);
    return;
  }

  printf("Enter elements:\n");
  Matrix *mat = &sys->list[sys->count - 1];
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      printf("[%d][%d]: ", i, j);
      read_double(&mat->data[i][j]);
    }
  }

  printf("\nMatrix %c created (%dx%d)\n\n", id, rows, cols);
}

void run_add_matrices(MatrixSystem *sys) {
  char id1, id2;
  Matrix *m1, *m2;
  Matrix result;

  list_available_matrices(sys);

  printf("\nMatrix 1 ID: ");
  read_char(&id1);
  printf("Matrix 2 ID: ");
  read_char(&id2);

  if (get_matrix_by_id(sys, id1, &m1) != SUCCESS ||
      get_matrix_by_id(sys, id2, &m2) != SUCCESS) {
    handle_error(ERR_MATRIX_NOT_FOUND);
    return;
  }

  Status status = add_matrices(m1, m2, &result);
  if (status == SUCCESS) {
    printf("\nResult (%c + %c):\n\n", id1, id2);
    print_matrix(&result);
    free_matrix_data(&result);
  } else {
    printf("\nError: Incompatible dimensions\n\n");
    printf("%c(%dx%d) + %c(%dx%d) \u274c\n\n", id1, m1->rows, m1->cols, id2,
           m2->rows, m2->cols);
  }
}

void run_multiply_matrices(MatrixSystem *sys) {
  char id1, id2;
  Matrix *m1, *m2;
  Matrix result;

  list_available_matrices(sys);

  printf("\nMatrix 1 ID: ");
  read_char(&id1);
  printf("Matrix 2 ID: ");
  read_char(&id2);

  if (get_matrix_by_id(sys, id1, &m1) != SUCCESS ||
      get_matrix_by_id(sys, id2, &m2) != SUCCESS) {
    handle_error(ERR_MATRIX_NOT_FOUND);
    return;
  }

  Status status = multiply_matrices(m1, m2, &result);
  if (status == SUCCESS) {
    printf("\nResult (%c * %c):\n\n", id1, id2);
    print_matrix(&result);
    free_matrix_data(&result);
  } else {
    printf("\nError: Incompatible dimensions for multiplication\n\n");
    printf("%c(%dx%d) x %c(%dx%d) \u274c\n\n", id1, m1->rows, m1->cols, id2,
           m2->rows, m2->cols);
    printf("Required: Cols of %c must equal Rows of %c\n\n", id1, id2);
  }
}

void run_transpose_matrix(MatrixSystem *sys) {
  char id;
  Matrix *src;
  Matrix dest;

  list_available_matrices(sys);

  printf("\nTranspose matrix ID: ");
  read_char(&id);

  if (get_matrix_by_id(sys, id, &src) != SUCCESS) {
    handle_error(ERR_MATRIX_NOT_FOUND);
    return;
  }

  if (transpose_matrix(src, &dest) == SUCCESS) {
    printf("\nMatrix %c^T (%dx%d):\n\n", id, dest.rows, dest.cols);
    print_matrix(&dest);
    free_matrix_data(&dest);
  }
}

void run_determinant(MatrixSystem *sys) {
  char id;
  Matrix *m;

  list_available_matrices(sys);

  printf("\nCalculate determinant for matrix ID: ");
  read_char(&id);

  if (get_matrix_by_id(sys, id, &m) != SUCCESS) {
    handle_error(ERR_MATRIX_NOT_FOUND);
    return;
  }

  if (m->rows != m->cols) {
    handle_error(ERR_NOT_SQUARE);
    return;
  }

  Result res = calculate_determinant(m->data, m->rows);
  if (res.status == SUCCESS) {
    printf("\nDeterminant |%c| = %.2f\n\n", id, res.value);
  } else {
    handle_error(res.status);
  }
}

void run_show_matrix(MatrixSystem *sys) {
  char id;
  Matrix *m;

  list_available_matrices(sys);

  printf("\nShow matrix ID: ");
  read_char(&id);

  if (get_matrix_by_id(sys, id, &m) != SUCCESS) {
    handle_error(ERR_MATRIX_NOT_FOUND);
    return;
  }

  printf("\nMatrix %c (%dx%d):\n\n", m->id, m->rows, m->cols);
  print_matrix(m);
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

Status read_double(double *value) {
  if (scanf("%lf", value) != 1) {
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

Status init_system(MatrixSystem *sys) {
  sys->list = (Matrix *)malloc(INITIAL_CAPACITY * sizeof(Matrix));
  if (sys->list == NULL) {
    return ERR_MEMORY_ALLOCATION;
  }

  sys->count = 0;
  sys->capacity = INITIAL_CAPACITY;

  return SUCCESS;
}

Status resize_system(MatrixSystem *sys) {
  if (sys->capacity >= MAX_CAPACITY) {
    return ERR_SYSTEM_FULL;
  }

  int new_cap = sys->capacity * 2;
  if (new_cap > MAX_CAPACITY) {
    new_cap = MAX_CAPACITY;
  }

  Matrix *new_list = (Matrix *)realloc(sys->list, new_cap * sizeof(Matrix));
  if (new_list == NULL) {
    return ERR_MEMORY_ALLOCATION;
  }

  sys->list = new_list;
  sys->capacity = new_cap;

  return SUCCESS;
}

void free_system(MatrixSystem *sys) {
  for (int i = 0; i < sys->count; i++) {
    free_matrix_data(&sys->list[i]);
  }

  free(sys->list);
  sys->list = NULL;
  sys->count = 0;
  sys->capacity = 0;
}

Status create_matrix(MatrixSystem *sys, char id, int rows, int cols) {
  Matrix *m = &sys->list[sys->count];
  m->id = id;
  m->rows = rows;
  m->cols = cols;

  m->data = (double **)malloc(rows * sizeof(double *));
  if (m->data == NULL) {
    return ERR_MEMORY_ALLOCATION;
  }

  for (int i = 0; i < rows; i++) {
    m->data[i] = (double *)malloc(cols * sizeof(double));
    if (m->data[i] == NULL) {
      for (int k = 0; k < i; k++)
        free(m->data[k]);
      free(m->data);
      return ERR_MEMORY_ALLOCATION;
    }
  }

  sys->count++;

  return SUCCESS;
}

void free_matrix_data(Matrix *mat) {
  if (mat->data) {
    for (int i = 0; i < mat->rows; i++) {
      free(mat->data[i]);
    }
    free(mat->data);
    mat->data = NULL;
  }
}

int find_matrix_index(const MatrixSystem *sys, char id) {
  for (int i = 0; i < sys->count; i++) {
    if (sys->list[i].id == id)
      return i;
  }

  return -1;
}

Status get_matrix_by_id(const MatrixSystem *sys, char id, Matrix **mat) {
  int idx = find_matrix_index(sys, id);
  if (idx == -1) {
    return ERR_MATRIX_NOT_FOUND;
  }

  *mat = &sys->list[idx];

  return SUCCESS;
}

Status add_matrices(const Matrix *a, const Matrix *b, Matrix *result) {
  if (a->rows != b->rows || a->cols != b->cols) {
    return ERR_INCOMPATIBLE_DIM;
  }

  result->rows = a->rows;
  result->cols = a->cols;
  result->data = (double **)malloc(result->rows * sizeof(double *));
  if (result->data == NULL) {
    return ERR_MEMORY_ALLOCATION;
  }

  for (int i = 0; i < result->rows; i++) {
    result->data[i] = (double *)malloc(result->cols * sizeof(double));
    if (result->data[i] == NULL) {
      for (int k = 0; k < i; k++)
        free(result->data[k]);
      free(result->data);
      return ERR_MEMORY_ALLOCATION;
    }

    for (int j = 0; j < result->cols; j++) {
      result->data[i][j] = a->data[i][j] + b->data[i][j];
    }
  }

  return SUCCESS;
}

Status multiply_matrices(const Matrix *a, const Matrix *b, Matrix *result) {
  if (a->cols != b->rows) {
    return ERR_INCOMPATIBLE_DIM;
  }

  result->rows = a->rows;
  result->cols = b->cols;
  result->data = (double **)malloc(result->rows * sizeof(double *));
  if (result->data == NULL) {
    return ERR_MEMORY_ALLOCATION;
  }

  for (int i = 0; i < result->rows; i++) {
    result->data[i] = (double *)malloc(result->cols * sizeof(double));
    if (result->data[i] == NULL) {
      for (int k = 0; k < i; k++)
        free(result->data[k]);
      free(result->data);
      return ERR_MEMORY_ALLOCATION;
    }
    for (int j = 0; j < result->cols; j++) {
      result->data[i][j] = 0;
      for (int k = 0; k < a->cols; k++) {
        result->data[i][j] += a->data[i][k] * b->data[k][j];
      }
    }
  }
  return SUCCESS;
}

Status transpose_matrix(const Matrix *src, Matrix *dest) {
  dest->rows = src->cols;
  dest->cols = src->rows;
  dest->data = (double **)malloc(dest->rows * sizeof(double *));
  if (dest->data == NULL) {
    return ERR_MEMORY_ALLOCATION;
  }

  for (int i = 0; i < dest->rows; i++) {
    dest->data[i] = (double *)malloc(dest->cols * sizeof(double));
    if (dest->data[i] == NULL) {
      for (int k = 0; k < i; k++)
        free(dest->data[k]);
      free(dest->data);
      return ERR_MEMORY_ALLOCATION;
    }
    for (int j = 0; j < dest->cols; j++) {
      dest->data[i][j] = src->data[j][i];
    }
  }

  return SUCCESS;
}

void get_cofactor(double **data, double **temp, int p, int q, int n) {
  int i = 0, j = 0;
  for (int row = 0; row < n; row++) {
    for (int col = 0; col < n; col++) {
      if (row != p && col != q) {
        temp[i][j++] = data[row][col];
        if (j == n - 1) {
          j = 0;
          i++;
        }
      }
    }
  }
}

Result calculate_determinant(double **data, int n) {
  Result res = {SUCCESS, 0.0};

  if (n == 1) {
    res.value = data[0][0];
    return res;
  }

  double **temp = (double **)malloc(n * sizeof(double *));
  if (temp == NULL) {
    res.status = ERR_MEMORY_ALLOCATION;
    return res;
  }

  for (int i = 0; i < n; i++) {
    temp[i] = (double *)malloc(n * sizeof(double));
    if (temp[i] == NULL) {
      for (int k = 0; k < i; k++) {
        free(temp[k]);
      }
      free(temp);

      res.status = ERR_MEMORY_ALLOCATION;

      return res;
    }
  }

  int sign = 1;
  for (int f = 0; f < n; f++) {
    get_cofactor(data, temp, 0, f, n);

    Result sub = calculate_determinant(temp, n - 1);
    if (sub.status != SUCCESS) {
      for (int i = 0; i < n; i++) {
        free(temp[i]);
      }
      free(temp);

      return sub;
    }

    res.value += sign * data[0][f] * sub.value;
    sign = -sign;
  }

  for (int i = 0; i < n; i++) {
    free(temp[i]);
  }
  free(temp);

  return res;
}
