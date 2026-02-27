/*
 ===============================================================================
 Exercise: 03_csv_parser.c
 Description: CSV File Parser and Manager
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - CSV File Reading and Parsing (handling commas)
 - Record Creation (Appending to CSV)
 - Record Searching and Filtering
 - Data Statistics Calculation directly from CSV data
 - Interactive menu and error handling
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "employees.csv"
#define TEMP_FILENAME "employees_tmp.csv"
#define MAX_LINE 256
#define MAX_NAME 50
#define MAX_DEPT 50
#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 6

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_FILE_NOT_FOUND,
  ERR_FILE_CREATE_FAILED,
  ERR_RECORD_NOT_FOUND,
  ERR_PARSE_ERROR
} Status;

typedef struct {
  int id;
  char name[MAX_NAME];
  char department[MAX_DEPT];
  float salary;
} Employee;

void show_menu(void);
void handle_error(Status status);

void run_display_all(void);
void run_add_record(void);
void run_search_record(void);
void run_calculate_stats(void);
void run_create_dummy_csv(void);

void clear_input_buffer(void);
Status read_integer(int *value);
Status read_float(float *value);
Status read_string(char *buffer, int max_len);

Status parse_csv_line(char *line, Employee *emp);

int main(void) {
  int option = 0;

  // Initialize a dummy CSV file if it doesn't exist
  run_create_dummy_csv();

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
      run_display_all();
      break;
    case 2:
      run_add_record();
      break;
    case 3:
      run_search_record();
      break;
    case 4:
      run_calculate_stats();
      break;
    case 5:
      // Reset file with dummy data
      remove(FILENAME);
      run_create_dummy_csv();
      printf("\n  - CSV file reset to default dummy data.\n\n");
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== CSV File Parser & Manager ===\n\n");
  printf("File: %s\n\n", FILENAME);
  printf("1. Display All Records\n");
  printf("2. Add New Record\n");
  printf("3. Search Record by ID\n");
  printf("4. Calculate Statistics\n");
  printf("5. Reset/Create Dummy CSV\n");
  printf("6. Exit\n");
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
    printf("Error: CSV file not found.\n\n");
    break;
  case ERR_FILE_CREATE_FAILED:
    printf("Error: Could not open or create CSV file.\n\n");
    break;
  case ERR_RECORD_NOT_FOUND:
    printf("Error: Record not found.\n\n");
    break;
  case ERR_PARSE_ERROR:
    printf("Error: Failed to parse CSV line format.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

Status parse_csv_line(char *line, Employee *emp) {
  // We expect: ID,Name,Department,Salary
  char *token;

  // Strip newline
  line[strcspn(line, "\n")] = 0;

  // Skip empty lines
  if (strlen(line) == 0)
    return ERR_PARSE_ERROR;

  // Parse ID
  token = strtok(line, ",");
  if (!token)
    return ERR_PARSE_ERROR;
  emp->id = atoi(token);

  // Parse Name
  token = strtok(NULL, ",");
  if (!token)
    return ERR_PARSE_ERROR;
  strncpy(emp->name, token, MAX_NAME - 1);
  emp->name[MAX_NAME - 1] = '\0';

  // Parse Department
  token = strtok(NULL, ",");
  if (!token)
    return ERR_PARSE_ERROR;
  strncpy(emp->department, token, MAX_DEPT - 1);
  emp->department[MAX_DEPT - 1] = '\0';

  // Parse Salary
  token = strtok(NULL, ",");
  if (!token)
    return ERR_PARSE_ERROR;
  emp->salary = atof(token);

  return SUCCESS;
}

void run_display_all(void) {
  FILE *file = fopen(FILENAME, "r");
  if (!file) {
    handle_error(ERR_FILE_NOT_FOUND);
    return;
  }

  char buffer[MAX_LINE];
  Employee emp;
  int count = 0;

  printf("\n=== Employee Records ===\n");
  printf("%-5s | %-20s | %-15s | %-10s\n", "ID", "Name", "Department",
         "Salary");
  printf("------------------------------------------------------------\n");

  // Read lines
  while (fgets(buffer, sizeof(buffer), file)) {
    // Skip header line if it exists
    if (count == 0 && strncmp(buffer, "ID,Name", 7) == 0) {
      continue;
    }

    if (parse_csv_line(buffer, &emp) == SUCCESS) {
      printf("%-5d | %-20s | %-15s | $%-9.2f\n", emp.id, emp.name,
             emp.department, emp.salary);
      count++;
    }
  }

  if (count == 0) {
    printf("  (No valid records found in file)\n");
  }

  printf("------------------------------------------------------------\n");
  printf("  - Total valid records parsed: %d\n\n", count);

  fclose(file);
}

void run_add_record(void) {
  Employee emp;

  printf("\n--- Add New Record ---\n");

  printf("ID: ");
  if (read_integer(&emp.id) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("Name: ");
  if (read_string(emp.name, MAX_NAME) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  // Basic sanity check: prevent commas in names for simple CSV format
  if (strchr(emp.name, ',')) {
    printf(
        "Error: Commas not allowed in input for this simple CSV format.\n\n");
    return;
  }

  printf("Department: ");
  if (read_string(emp.department, MAX_DEPT) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  if (strchr(emp.department, ',')) {
    printf(
        "Error: Commas not allowed in input for this simple CSV format.\n\n");
    return;
  }

  printf("Salary: ");
  if (read_float(&emp.salary) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  FILE *file = fopen(FILENAME, "a");
  if (!file) {
    handle_error(ERR_FILE_CREATE_FAILED);
    return;
  }

  // Check if file is completely empty (needs header)
  fseek(file, 0, SEEK_END);
  if (ftell(file) == 0) {
    fprintf(file, "ID,Name,Department,Salary\n");
  }

  // Note: if file doesn't end with newline, this might append to same line,
  // but we enforce newlines in our writes.
  fprintf(file, "%d,%s,%s,%.2f\n", emp.id, emp.name, emp.department,
          emp.salary);
  fclose(file);

  printf("\n  - Record added successfully to %s\n\n", FILENAME);
}

void run_search_record(void) {
  int search_id;
  printf("\nEnter Employee ID to search: ");
  if (read_integer(&search_id) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  FILE *file = fopen(FILENAME, "r");
  if (!file) {
    handle_error(ERR_FILE_NOT_FOUND);
    return;
  }

  char buffer[MAX_LINE];
  Employee emp;
  int found = 0;
  int is_first_line = 1;

  while (fgets(buffer, sizeof(buffer), file)) {
    if (is_first_line && strncmp(buffer, "ID,Name", 7) == 0) {
      is_first_line = 0;
      continue;
    }
    is_first_line = 0;

    if (parse_csv_line(buffer, &emp) == SUCCESS) {
      if (emp.id == search_id) {
        printf("\n=== Record Found ===\n");
        printf("  - ID:         %d\n", emp.id);
        printf("  - Name:       %s\n", emp.name);
        printf("  - Department: %s\n", emp.department);
        printf("  - Salary:     $%.2f\n\n", emp.salary);
        found = 1;
        break;
      }
    }
  }

  if (!found) {
    handle_error(ERR_RECORD_NOT_FOUND);
  }

  fclose(file);
}

void run_calculate_stats(void) {
  FILE *file = fopen(FILENAME, "r");
  if (!file) {
    handle_error(ERR_FILE_NOT_FOUND);
    return;
  }

  char buffer[MAX_LINE];
  Employee emp;
  int count = 0;
  float total_salary = 0.0f;
  float max_salary = -1.0f;
  char max_earner[MAX_NAME] = "";
  int is_first_line = 1;

  while (fgets(buffer, sizeof(buffer), file)) {
    if (is_first_line && strncmp(buffer, "ID,Name", 7) == 0) {
      is_first_line = 0;
      continue;
    }
    is_first_line = 0;

    if (parse_csv_line(buffer, &emp) == SUCCESS) {
      count++;
      total_salary += emp.salary;
      if (emp.salary > max_salary) {
        max_salary = emp.salary;
        strcpy(max_earner, emp.name);
      }
    }
  }

  fclose(file);

  printf("\n=== CSV Statistics ===\n");
  if (count > 0) {
    printf("  - Total Employees: %d\n", count);
    printf("  - Avg Salary:      $%.2f\n", total_salary / count);
    printf("  - Total Payroll:   $%.2f\n", total_salary);
    printf("  - Highest Earner:  %s ($%.2f)\n\n", max_earner, max_salary);
  } else {
    printf("  - No data available to calculate statistics.\n\n");
  }
}

void run_create_dummy_csv(void) {
  FILE *check = fopen(FILENAME, "r");
  if (check) {
    fclose(check);
    return; // File already exists
  }

  FILE *file = fopen(FILENAME, "w");
  if (file) {
    fputs("ID,Name,Department,Salary\n", file);
    fputs("101,Ada Lovelace,Engineering,125000.00\n", file);
    fputs("102,Alan Turing,Research,135000.50\n", file);
    fputs("103,Grace Hopper,Management,145000.00\n", file);
    fputs("104,Edgar Codd,Database,115000.75\n", file);
    fclose(file);
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
