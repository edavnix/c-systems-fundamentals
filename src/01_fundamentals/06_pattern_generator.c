/*
 ===============================================================================
 Exercise: 06_pattern_generator.c
 Description: ASCII pattern generator (Pyramid, Diamond, etc.)
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Geometric pattern generation using nested loops
 - Configurable height and character
 - Symmetric pattern logic
 - Mathematical area approximation
 ===============================================================================
*/

#include <stdio.h>

#define TRUE 1
#define MIN_OPTION 1
#define MAX_OPTION 6
#define MIN_HEIGHT 1
#define MAX_HEIGHT 50

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_OUT_OF_RANGE,
  ERR_INVALID_OPTION
} Status;

typedef struct {
  Status status;
  int area;
  const char *symmetry;
} PatternDisplay;

void show_menu(void);
void handle_error(Status status);
void show_pattern_stats(int option, int height, PatternDisplay *stats);
void run_pattern_generation(int option);

void clear_input_buffer(void);
Status read_integer(int *value);
Status read_char(char *value);

void draw_triangle(int h, char c);
void draw_inverted_triangle(int h, char c);
void draw_diamond(int h, char c);
void draw_right_triangle(int h, char c);
void draw_square(int h, char c);

int main(void) {
  int option = 0;

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nThank you for using the generator!\n");
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    run_pattern_generation(option);
  }

  return 0;
}

void show_menu(void) {
  printf("=== Pattern Generator ===\n\n");
  printf("1. Triangle\n2. Inverted Triangle\n3. Diamond\n"
         "4. Right Triangle\n5. Square\n6. Exit\n");
  printf("Select pattern: ");
}

void handle_error(Status status) {
  switch (status) {
  case ERR_INVALID_INPUT:
    printf("Error: Invalid input. Please try again.\n\n");
    break;
  case ERR_OUT_OF_RANGE:
    printf("Error: Height out of range (%d-%d).\n\n", MIN_HEIGHT, MAX_HEIGHT);
    break;
  case ERR_INVALID_OPTION:
    printf("Error: Invalid option selection.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void show_pattern_stats(int option, int height, PatternDisplay *stats) {
  stats->status = SUCCESS;
  stats->area = 0;
  stats->symmetry = "None";

  switch (option) {
  case 1:
    stats->area = height * height;
    stats->symmetry = "Vertical";
    break;
  case 2:
    stats->area = (int)(height * height * 0.5);
    stats->symmetry = "Vertical";
    break;
  case 3:
    stats->area = (int)(height * height * 0.5);
    stats->symmetry = "Vertical, Horizontal";
    break;
  case 4:
    stats->area = height * (height + 1) / 2;
    stats->symmetry = "None";
    break;
  case 5:
    stats->area = height * height;
    stats->symmetry = "Vertical, Horizontal";
    break;
  default:
    stats->status = ERR_INVALID_OPTION;
    break;
  }
}

void run_pattern_generation(int option) {
  int height;
  char character;

  printf("\nEnter height (%d-%d): ", MIN_HEIGHT, MAX_HEIGHT);
  if (read_integer(&height) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  if (height < MIN_HEIGHT || height > MAX_HEIGHT) {
    handle_error(ERR_OUT_OF_RANGE);
    return;
  }

  if (option == 3 && height % 2 == 0) {
    height++;
    printf("Note: Diamond requires odd height. Adjusted to %d.\n", height);
  }

  printf("Enter character: ");
  if (read_char(&character) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("\n");
  switch (option) {
  case 1:
    draw_triangle(height, character);
    break;
  case 2:
    draw_inverted_triangle(height, character);
    break;
  case 3:
    draw_diamond(height, character);
    break;
  case 4:
    draw_right_triangle(height, character);
    break;
  case 5:
    draw_square(height, character);
    break;
  }

  PatternDisplay stats;
  show_pattern_stats(option, height, &stats);
  if (stats.status == SUCCESS) {
    printf("\n  - Approximate area: %d characters\n", stats.area);
    printf("  - Symmetry lines: %s\n\n", stats.symmetry);
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

Status read_char(char *value) {
  if (scanf(" %c", value) != 1) {
    clear_input_buffer();
    return ERR_INVALID_INPUT;
  }
  clear_input_buffer();
  return SUCCESS;
}

void draw_triangle(int h, char c) {
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < h - i - 1; j++) {
      printf(" ");
    }
    for (int j = 0; j < (2 * i + 1); j++) {
      printf("%c", c);
    }
    printf("\n");
  }
}

void draw_inverted_triangle(int h, char c) {
  for (int i = h; i >= 1; i--) {
    for (int j = 0; j < h - i; j++) {
      printf(" ");
    }
    for (int j = 0; j < (2 * i - 1); j++) {
      printf("%c", c);
    }
    printf("\n");
  }
}

void draw_diamond(int h, char c) {
  int half = h / 2;

  for (int i = 0; i <= half; i++) {
    for (int j = 0; j < half - i; j++) {
      printf(" ");
    }
    for (int j = 0; j < (2 * i + 1); j++) {
      printf("%c", c);
    }
    printf("\n");
  }

  for (int i = half - 1; i >= 0; i--) {
    for (int j = 0; j < half - i; j++) {
      printf(" ");
    }
    for (int j = 0; j < (2 * i + 1); j++) {
      printf("%c", c);
    }
    printf("\n");
  }
}

void draw_right_triangle(int h, char c) {
  for (int i = 1; i <= h; i++) {
    for (int j = 0; j < i; j++) {
      printf("%-2c", c);
    }
    printf("\n");
  }
}

void draw_square(int h, char c) {
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < h; j++) {
      printf("%-2c", c);
    }
    printf("\n");
  }
}
