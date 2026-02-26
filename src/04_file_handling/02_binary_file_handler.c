/*
 ===============================================================================
 Exercise: 02_binary_file_handler.c
 Description: Binary File CRUD Operations (Create, Read, Update, Delete)
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Binary File I/O (rb, wb, ab, r+b)
 - Fixed-size record structures (Exactly 32 bytes)
 - Direct access using fseek() and ftell()
 - Data statistics calculation directly from binary stream
 - Fully implemented Delete operation using temp file
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "estudiantes.dat"
#define TEMP_FILENAME "estudiantes_tmp.dat"
#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 7

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_FILE_NOT_FOUND,
  ERR_FILE_CREATE_FAILED,
  ERR_RECORD_NOT_FOUND
} Status;

/*
 * Estructura de 32 bytes exactos
 * (4 int + 20 char + 4 int + 4 float = 32 bytes)
 * Note: struct padding might affect actual size, packing is recommended for
 * strict binary
 */
#pragma pack(push, 1)
typedef struct {
  int id;
  char nombre[20];
  int edad;
  float promedio;
} Student;
#pragma pack(pop)

void show_menu(void);
void handle_error(Status status);

void run_create_record(void);
void run_read_all_records(void);
void run_search_record(void);
void run_update_record(void);
void run_delete_record(void);
void run_show_statistics(void);

void clear_input_buffer(void);
Status read_integer(int *value);
Status read_float(float *value);
Status read_string(char *buffer, int max_len);

int main(void) {
  int option = 0;

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
      run_create_record();
      break;
    case 2:
      run_read_all_records();
      break;
    case 3:
      run_search_record();
      break;
    case 4:
      run_update_record();
      break;
    case 5:
      run_delete_record();
      break;
    case 6:
      run_show_statistics();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Binary File Manager (CRUD) ===\n\n");
  printf("File: %s\n\n", FILENAME);
  printf("1. Create Record\n");
  printf("2. Read All Records\n");
  printf("3. Search Record by ID\n");
  printf("4. Update Record\n");
  printf("5. Delete Record\n");
  printf("6. Statistics\n");
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
  case ERR_FILE_NOT_FOUND:
    printf("Error: Data file not found or empty.\n\n");
    break;
  case ERR_FILE_CREATE_FAILED:
    printf("Error: Could not open file for writing.\n\n");
    break;
  case ERR_RECORD_NOT_FOUND:
    printf("Error: Record not found.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_create_record(void) {
  Student s;
  FILE *file = fopen(FILENAME, "ab");
  if (!file) {
    handle_error(ERR_FILE_CREATE_FAILED);
    return;
  }

  printf("\n--- Create New Record ---\n");
  printf("ID: ");
  if (read_integer(&s.id) != SUCCESS) {
    fclose(file);
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("Name: ");
  if (read_string(s.nombre, sizeof(s.nombre)) != SUCCESS) {
    fclose(file);
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("Age: ");
  if (read_integer(&s.edad) != SUCCESS) {
    fclose(file);
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("GPA (Promedio): ");
  if (read_float(&s.promedio) != SUCCESS) {
    fclose(file);
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  fwrite(&s, sizeof(Student), 1, file);
  fclose(file);

  printf("\n  - Record saved successfully to %s\n\n", FILENAME);
}

void run_read_all_records(void) {
  Student s;
  FILE *file = fopen(FILENAME, "rb");
  if (!file) {
    handle_error(ERR_FILE_NOT_FOUND);
    return;
  }

  printf("\n--- All Records (%s) ---\n", FILENAME);

  int count = 0;
  while (fread(&s, sizeof(Student), 1, file)) {
    printf("  [%d] ID: %-5d | Name: %-15s | Age: %-3d | GPA: %.1f\n", count + 1,
           s.id, s.nombre, s.edad, s.promedio);
    count++;
  }

  if (count == 0) {
    printf("  (No records found)\n");
  }

  fseek(file, 0, SEEK_END);
  long total_bytes = ftell(file);
  fclose(file);

  printf("\n  - Total: %d records (%ld bytes)\n\n", count, total_bytes);
}

void run_search_record(void) {
  int search_id;
  printf("\nEnter ID to search: ");
  if (read_integer(&search_id) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Student s;
  FILE *file = fopen(FILENAME, "rb");
  if (!file) {
    handle_error(ERR_FILE_NOT_FOUND);
    return;
  }

  int found = 0;
  long position = 0;

  while (fread(&s, sizeof(Student), 1, file)) {
    if (s.id == search_id) {
      found = 1;
      position = ftell(file) - sizeof(Student);
      break;
    }
  }

  if (found) {
    printf("\n=== Record Found ===\n");
    printf("  - Name: %s\n", s.nombre);
    printf("  - Age:  %d\n", s.edad);
    printf("  - GPA:  %.1f\n", s.promedio);
    printf("  - File Position: byte %ld\n\n", position);
  } else {
    handle_error(ERR_RECORD_NOT_FOUND);
  }

  fclose(file);
}

void run_update_record(void) {
  int update_id;
  printf("\nEnter ID to update: ");
  if (read_integer(&update_id) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Student s;
  FILE *file = fopen(FILENAME, "r+b");
  if (!file) {
    handle_error(ERR_FILE_NOT_FOUND);
    return;
  }

  int found = 0;
  while (fread(&s, sizeof(Student), 1, file)) {
    if (s.id == update_id) {
      found = 1;
      printf("Record found. Current GPA: %.1f\n", s.promedio);
      printf("New GPA: ");

      if (read_float(&s.promedio) == SUCCESS) {
        // Move pointer back to overwrite this exact record
        fseek(file, -sizeof(Student), SEEK_CUR);
        fwrite(&s, sizeof(Student), 1, file);
        printf("\n  - Record updated successfully.\n\n");
      } else {
        handle_error(ERR_INVALID_INPUT);
      }
      break;
    }
  }

  if (!found) {
    handle_error(ERR_RECORD_NOT_FOUND);
  }

  fclose(file);
}

void run_delete_record(void) {
  int delete_id;
  printf("\nEnter ID to delete: ");
  if (read_integer(&delete_id) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  FILE *file = fopen(FILENAME, "rb");
  if (!file) {
    handle_error(ERR_FILE_NOT_FOUND);
    return;
  }

  FILE *temp_file = fopen(TEMP_FILENAME, "wb");
  if (!temp_file) {
    fclose(file);
    handle_error(ERR_FILE_CREATE_FAILED);
    return;
  }

  Student s;
  int found = 0;
  int records_kept = 0;

  while (fread(&s, sizeof(Student), 1, file)) {
    if (s.id == delete_id) {
      found = 1;
    } else {
      fwrite(&s, sizeof(Student), 1, temp_file);
      records_kept++;
    }
  }

  fclose(file);
  fclose(temp_file);

  if (found) {
    remove(FILENAME);
    rename(TEMP_FILENAME, FILENAME);
    printf("\n  - Record deleted successfully.\n");
    printf("  - Remaining records: %d\n\n", records_kept);
  } else {
    remove(TEMP_FILENAME);
    handle_error(ERR_RECORD_NOT_FOUND);
  }
}

void run_show_statistics(void) {
  Student s;
  FILE *file = fopen(FILENAME, "rb");
  if (!file) {
    handle_error(ERR_FILE_NOT_FOUND);
    return;
  }

  int count = 0;
  float sum_promedios = 0.0f;
  int sum_edades = 0;

  while (fread(&s, sizeof(Student), 1, file)) {
    count++;
    sum_promedios += s.promedio;
    sum_edades += s.edad;
  }

  fseek(file, 0, SEEK_END);
  long total_bytes = ftell(file);
  fclose(file);

  printf("\n=== Statistics for %s ===\n", FILENAME);

  if (count > 0) {
    printf("  - File Size:       %ld bytes\n", total_bytes);
    printf("  - Total Records:   %d\n", count);
    printf("  - Size per Record: %zu bytes\n", sizeof(Student));

    float avg_promedio = sum_promedios / count;
    float avg_edad = (float)sum_edades / count;

    printf("  - Average GPA:     %.2f\n", avg_promedio);
    printf("  - Average Age:     %.2f years\n\n", avg_edad);
  } else {
    printf("  - No records to analyze.\n\n");
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

Status read_float(float *value) {
  if (scanf("%f", value) != 1) {
    clear_input_buffer();
    return ERR_INVALID_INPUT;
  }
  clear_input_buffer();

  return SUCCESS;
}

Status read_string(char *buffer, int max_len) {
  if (fgets(buffer, max_len, stdin) == NULL) {
    return ERR_INVALID_INPUT;
  }
  size_t len = strlen(buffer);
  if (len > 0 && buffer[len - 1] == '\n') {
    buffer[len - 1] = '\0';
  } else if (len == (size_t)(max_len - 1)) {
    clear_input_buffer();
  }

  return SUCCESS;
}
