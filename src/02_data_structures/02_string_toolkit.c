/*
 ===============================================================================
 Exercise: 02_string_toolkit.c
 Description: Custom implementation of standard string operations
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Manual implementation of string logic (no <string.h> logic used)
 - Length, Copy, Concatenate, Compare
 - Reverse, Upper/Lower case, Substring extraction
 - Buffer safety checks and input validation
 ===============================================================================
*/

#include <stdio.h>

#define TRUE 1
#define MIN_OPTION 1
#define MAX_OPTION 9
#define MAX_BUFFER 256

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_BUFFER_OVERFLOW,
  ERR_INDEX_OUT_OF_BOUNDS
} Status;

typedef struct {
  Status status;
  int value;
} Result;

void show_menu(void);
void handle_error(Status status);
void run_length(void);
void run_copy(void);
void run_concat(void);
void run_compare(void);
void run_reverse(void);
void run_to_upper(void);
void run_to_lower(void);
void run_substring(void);

void clear_input_buffer(void);
Status read_integer(int *value);
void read_string(char *buffer, int max_length);

Result custom_strlen(const char *str);
void custom_strcpy(char *dest, const char *src);
Status custom_strcat(char *dest, const char *src, int max_dest_size);
Result custom_strcmp(const char *s1, const char *s2);
void custom_reverse(char *str);
void custom_to_upper(char *str);
void custom_to_lower(char *str);
Status custom_substring(const char *src, int start, int length, char *dest);

int main(void) {
  int option = 0;

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nExiting toolkit...\n");
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    switch (option) {
    case 1:
      run_length();
      break;
    case 2:
      run_copy();
      break;
    case 3:
      run_concat();
      break;
    case 4:
      run_compare();
      break;
    case 5:
      run_reverse();
      break;
    case 6:
      run_to_upper();
      break;
    case 7:
      run_to_lower();
      break;
    case 8:
      run_substring();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== String Toolkit (Custom Implementation) ===\n\n");
  printf("1. Length (strlen)\n2. Copy (strcpy)\n3. Concatenate (strcat)\n"
         "4. Compare (strcmp)\n5. Reverse\n6. Convert to Uppercase\n"
         "7. Convert to Lowercase\n8. Extract Substring\n9. Exit\n");
  printf("Option: ");
}

void handle_error(Status status) {
  switch (status) {
  case ERR_INVALID_INPUT:
    printf("Error: Invalid input. Please enter a valid number.\n\n");
    break;
  case ERR_INVALID_OPTION:
    printf("Error: Invalid option selected.\n\n");
    break;
  case ERR_BUFFER_OVERFLOW:
    printf("Error: Operation would overflow the buffer.\n\n");
    break;
  case ERR_INDEX_OUT_OF_BOUNDS:
    printf("Error: Index or length out of bounds.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_length(void) {
  char buffer[MAX_BUFFER];
  printf("\nEnter string: ");
  read_string(buffer, MAX_BUFFER);

  Result res = custom_strlen(buffer);
  if (res.status == SUCCESS) {
    printf("\n  - Length: %d characters\n\n", res.value);
  }
}

void run_copy(void) {
  char source[MAX_BUFFER];
  char dest[MAX_BUFFER];

  printf("\nSource string: ");
  read_string(source, MAX_BUFFER);

  custom_strcpy(dest, source);
  printf("\n  - Copied string: %s\n\n", dest);
}

void run_concat(void) {
  char str1[MAX_BUFFER];
  char str2[MAX_BUFFER];

  printf("\nString 1: ");
  read_string(str1, MAX_BUFFER);
  printf("String 2: ");
  read_string(str2, MAX_BUFFER);

  Status status = custom_strcat(str1, str2, MAX_BUFFER);
  if (status == SUCCESS) {
    printf("\n  - Result: %s\n", str1);
    printf("  - Final Length: %d\n\n", custom_strlen(str1).value);
  } else {
    handle_error(status);
  }
}

void run_compare(void) {
  char str1[MAX_BUFFER];
  char str2[MAX_BUFFER];

  printf("\nString 1: ");
  read_string(str1, MAX_BUFFER);
  printf("String 2: ");
  read_string(str2, MAX_BUFFER);

  Result res = custom_strcmp(str1, str2);
  if (res.status == SUCCESS) {
    printf("\n  - Numerical result: %d\n", res.value);
    if (res.value == 0) {
      printf("  - Analysis: Strings are identical.\n\n");
    } else if (res.value < 0) {
      printf("  - Analysis: String 1 is alphabetically SMALLER than String "
             "2.\n\n");
    } else {
      printf("  - Analysis: String 1 is alphabetically GREATER than String "
             "2.\n\n");
    }
  }
}

void run_reverse(void) {
  char str[MAX_BUFFER];
  printf("\nString to reverse: ");
  read_string(str, MAX_BUFFER);

  custom_reverse(str);
  printf("\n  - Result: %s\n\n", str);
}

void run_to_upper(void) {
  char str[MAX_BUFFER];
  printf("\nString to uppercase: ");
  read_string(str, MAX_BUFFER);

  custom_to_upper(str);
  printf("\n  - Result: %s\n\n", str);
}

void run_to_lower(void) {
  char str[MAX_BUFFER];
  printf("\nString to lowercase: ");
  read_string(str, MAX_BUFFER);

  custom_to_lower(str);
  printf("\n  - Result: %s\n\n", str);
}

void run_substring(void) {
  char source[MAX_BUFFER];
  char dest[MAX_BUFFER];
  int start, length;

  printf("\nString: ");
  read_string(source, MAX_BUFFER);

  printf("Start index: ");
  if (read_integer(&start) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("Length: ");
  if (read_integer(&length) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Status status = custom_substring(source, start, length, dest);
  if (status == SUCCESS) {
    printf("\n  - Substring: %s\n\n", dest);
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

Status read_integer(int *value) {
  if (scanf("%d", value) != 1) {
    clear_input_buffer();
    return ERR_INVALID_INPUT;
  }
  clear_input_buffer();
  return SUCCESS;
}

void read_string(char *buffer, int max_length) {
  int c, i = 0;
  while (i < max_length - 1 && (c = getchar()) != '\n' && c != EOF) {
    buffer[i++] = (char)c;
  }
  buffer[i] = '\0';

  if (i == max_length - 1 && c != '\n' && c != EOF) {
    clear_input_buffer();
  }
}

Result custom_strlen(const char *str) {
  Result res = {SUCCESS, 0};
  while (str[res.value] != '\0') {
    res.value++;
  }

  return res;
}

void custom_strcpy(char *dest, const char *src) {
  int i = 0;
  while (src[i] != '\0') {
    dest[i] = src[i];
    i++;
  }
  dest[i] = '\0';
}

Status custom_strcat(char *dest, const char *src, int max_dest_size) {
  int dest_len = custom_strlen(dest).value;
  int src_len = custom_strlen(src).value;

  if (dest_len + src_len >= max_dest_size) {
    return ERR_BUFFER_OVERFLOW;
  }

  int i = 0;
  while (src[i] != '\0') {
    dest[dest_len + i] = src[i];
    i++;
  }
  dest[dest_len + i] = '\0';

  return SUCCESS;
}

Result custom_strcmp(const char *s1, const char *s2) {
  Result res = {SUCCESS, 0};
  int i = 0;

  while (s1[i] != '\0' && s2[i] != '\0') {
    if (s1[i] != s2[i]) {
      res.value = s1[i] - s2[i];
      return res;
    }
    i++;
  }

  res.value = s1[i] - s2[i];

  return res;
}

void custom_reverse(char *str) {
  int len = custom_strlen(str).value;
  int start = 0;
  int end = len - 1;
  char temp;

  while (start < end) {
    temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    start++;
    end--;
  }
}

void custom_to_upper(char *str) {
  int i = 0;
  while (str[i] != '\0') {
    if (str[i] >= 'a' && str[i] <= 'z') {
      str[i] = str[i] - 32;
    }
    i++;
  }
}

void custom_to_lower(char *str) {
  int i = 0;
  while (str[i] != '\0') {
    if (str[i] >= 'A' && str[i] <= 'Z') {
      str[i] = str[i] + 32;
    }
    i++;
  }
}

Status custom_substring(const char *src, int start, int length, char *dest) {
  int src_len = custom_strlen(src).value;

  if (start < 0 || start >= src_len || length < 0) {
    return ERR_INDEX_OUT_OF_BOUNDS;
  }

  int i;
  for (i = 0; i < length && (start + i) < src_len; i++) {
    dest[i] = src[start + i];
  }
  dest[i] = '\0';

  return SUCCESS;
}
