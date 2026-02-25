/*
 ===============================================================================
 Exercise: 01_text_file_processor.c
 Description: Text File Processor (Read, Search, Replace, Stats)
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - File I/O operations (fopen, fgets, fputs, etc.)
 - Substring search within lines
 - Word replacement and file updating
 - File statistics calculation
 - Interactive menu and error handling
 ===============================================================================
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024
#define FILENAME "datos.txt"
#define TEMP_FILENAME "temp.txt"
#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 7

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_FILE_NOT_FOUND,
  ERR_FILE_CREATE_FAILED
} Status;

void show_menu(void);
void handle_error(Status status);

void run_read_file(void);
void run_write_file(void);
void run_append_file(void);
void run_search_word(void);
void run_replace_word(void);
void run_statistics(void);

void clear_input_buffer(void);
Status read_integer(int *value);
Status read_string(char *buffer, int max_len);

void create_dummy_file(void);

int main(void) {
  int option = 0;

  // Initialize a demo file if it doesn't exist
  create_dummy_file();

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
      run_read_file();
      break;
    case 2:
      run_write_file();
      break;
    case 3:
      run_append_file();
      break;
    case 4:
      run_search_word();
      break;
    case 5:
      run_replace_word();
      break;
    case 6:
      run_statistics();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Text File Processor ===\n\n");
  printf("1. Read File\n");
  printf("2. Write/Create File\n");
  printf("3. Append Content\n");
  printf("4. Search Word\n");
  printf("5. Replace Word\n");
  printf("6. File Statistics\n");
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
    printf("Error: Could not open file for reading.\n\n");
    break;
  case ERR_FILE_CREATE_FAILED:
    printf("Error: Could not create or open file for writing.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_read_file(void) {
  char filepath[256];
  printf("\nEnter file path (leave empty for default '%s'): ", FILENAME);

  if (read_string(filepath, sizeof(filepath)) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  if (strlen(filepath) == 0) {
    strcpy(filepath, FILENAME);
  }

  FILE *file = fopen(filepath, "r");
  if (!file) {
    handle_error(ERR_FILE_NOT_FOUND);
    return;
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  rewind(file);

  printf("\n  - File opened successfully (%ld bytes)\n\n", size);
  printf("=== Content of %s ===\n", filepath);

  char buffer[MAX_LINE];
  while (fgets(buffer, sizeof(buffer), file)) {
    printf("%s", buffer);
  }
  printf("\n=========================\n\n");

  fclose(file);
}

void run_write_file(void) {
  printf("\n--- Write/Create File ---\n");
  FILE *file = fopen(FILENAME, "w");
  if (!file) {
    handle_error(ERR_FILE_CREATE_FAILED);
    return;
  }

  printf("Enter content to write to '%s' (Type 'EOF' on a new line to stop):\n",
         FILENAME);
  char buffer[MAX_LINE];

  while (TRUE) {
    printf("> ");
    if (read_string(buffer, sizeof(buffer)) != SUCCESS) {
      continue;
    }

    if (strcmp(buffer, "EOF") == 0) {
      break;
    }

    fprintf(file, "%s\n", buffer);
  }

  fclose(file);
  printf("\n  - File written successfully.\n\n");
}

void run_append_file(void) {
  printf("\n--- Append Content ---\n");
  FILE *file = fopen(FILENAME, "a");
  if (!file) {
    handle_error(ERR_FILE_CREATE_FAILED);
    return;
  }

  printf(
      "Enter content to append to '%s' (Type 'EOF' on a new line to stop):\n",
      FILENAME);
  char buffer[MAX_LINE];

  while (TRUE) {
    printf("> ");
    if (read_string(buffer, sizeof(buffer)) != SUCCESS) {
      continue;
    }

    if (strcmp(buffer, "EOF") == 0) {
      break;
    }

    fprintf(file, "%s\n", buffer);
  }

  fclose(file);
  printf("\n  - Content appended successfully.\n\n");
}

void run_search_word(void) {
  char word[256];
  printf("\nEnter word to search for: ");
  if (read_string(word, sizeof(word)) != SUCCESS || strlen(word) == 0) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  FILE *file = fopen(FILENAME, "r");
  if (!file) {
    handle_error(ERR_FILE_NOT_FOUND);
    return;
  }

  char buffer[MAX_LINE];
  int line_num = 1;
  int occurrences = 0;

  printf("\n=== Search Results for \"%s\" ===\n", word);
  while (fgets(buffer, sizeof(buffer), file)) {
    char *pos = buffer;
    while ((pos = strstr(pos, word)) != NULL) {
      if (pos == buffer) {
        printf("  - Line %d: %s", line_num, buffer);
        if (buffer[strlen(buffer) - 1] != '\n')
          printf("\n");
      }
      occurrences++;
      pos += strlen(word);
    }
    line_num++;
  }

  printf("\n  - Total occurrences: %d\n\n", occurrences);
  fclose(file);
}

void run_replace_word(void) {
  char old_word[256], new_word[256];

  printf("\nEnter word to replace: ");
  if (read_string(old_word, sizeof(old_word)) != SUCCESS ||
      strlen(old_word) == 0) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("Enter new word: ");
  if (read_string(new_word, sizeof(new_word)) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  FILE *file = fopen(FILENAME, "r");
  if (!file) {
    handle_error(ERR_FILE_NOT_FOUND);
    return;
  }

  FILE *temp_file = fopen(TEMP_FILENAME, "w");
  if (!temp_file) {
    fclose(file);
    handle_error(ERR_FILE_CREATE_FAILED);
    return;
  }

  char buffer[MAX_LINE];
  int replacements = 0;

  while (fgets(buffer, sizeof(buffer), file)) {
    char *pos, *start = buffer;
    while ((pos = strstr(start, old_word)) != NULL) {
      // Print everything before old_word
      fwrite(start, 1, pos - start, temp_file);
      // Print new_word
      fputs(new_word, temp_file);
      replacements++;
      start = pos + strlen(old_word);
    }
    // Print the rest of the line
    fputs(start, temp_file);
  }

  fclose(file);
  fclose(temp_file);

  remove(FILENAME);
  rename(TEMP_FILENAME, FILENAME);

  printf("\n  - Replacements made: %d\n", replacements);
  printf("  - File updated successfully.\n\n");
}

void run_statistics(void) {
  FILE *file = fopen(FILENAME, "r");
  if (!file) {
    handle_error(ERR_FILE_NOT_FOUND);
    return;
  }

  int chars = 0, words = 0, lines = 0;
  char ch, prev_ch = ' ';

  while ((ch = fgetc(file)) != EOF) {
    chars++;
    if (ch == '\n') {
      lines++;
    }

    if (isspace(ch) && !isspace(prev_ch)) {
      words++;
    }
    prev_ch = ch;
  }

  // Handle last word and EOF without newline
  if (chars > 0 && !isspace(prev_ch)) {
    words++;
  }
  if (chars > 0 && prev_ch != '\n') {
    lines++;
  }

  printf("\n=== Statistics for '%s' ===\n", FILENAME);
  printf("  - Total Characters: %d\n", chars);
  printf("  - Total Words:      %d\n", words);
  printf("  - Total Lines:      %d\n", lines);

  if (lines > 0) {
    printf("  - Avg Words/Line:   %.2f\n", (double)words / lines);
  }
  printf("\n");

  fclose(file);
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

void create_dummy_file(void) {
  FILE *check = fopen(FILENAME, "r");
  if (check) {
    fclose(check);
    return; // File already exists
  }

  FILE *file = fopen(FILENAME, "w");
  if (file) {
    fputs("The quick brown fox jumps over the lazy dog.\n", file);
    fputs("This is a test file for processing.\n", file);
    fputs("C programming is powerful and efficient.\n", file);
    fclose(file);
  }
}
