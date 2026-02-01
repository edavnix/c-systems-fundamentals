/*
 ===============================================================================
 Exercise: 07_number_guessing_game.c
 Description: Number guessing game with logic hints and statistics
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Random number generation
 - Logic hints (Higher/Lower)
 - Dynamic range updates
 - Session statistics tracking (wins, attempts, efficiency)
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 2
#define DEFAULT_MIN 1
#define DEFAULT_MAX 100
#define MAX_ATTEMPTS 7

typedef enum { SUCCESS, ERR_INVALID_INPUT, ERR_INVALID_OPTION } Status;

typedef struct {
  int games_played;
  int games_won;
  int total_attempts;
} SessionStats;

typedef struct {
  Status status;
  int won;
  int attempts;
  double efficiency;
} GameResult;

void show_menu(void);
void show_stats(const SessionStats *stats);
void handle_error(Status status);
void run_game_session(SessionStats *stats);

void clear_input_buffer(void);
Status read_integer(int *value);

GameResult process_game_session(int secret_number);
int generate_secret_number(int min, int max);

int main(void) {
  int option = 0;
  SessionStats stats = {0, 0, 0};

  srand((unsigned int)time(NULL));

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nThank you for playing!\n");
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    if (option == 1) {
      run_game_session(&stats);
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Smart Guessing Game ===\n\n");
  printf("1. Play New Game\n2. Exit\n");
  printf("Option: ");
}

void show_stats(const SessionStats *stats) {
  printf("\n------ Session Statistics ------\n\n");
  printf("  - Games played: %d\n", stats->games_played);
  printf("  - Victories: %d\n", stats->games_won);

  if (stats->games_played > 0) {
    double avg = (double)stats->total_attempts / stats->games_played;
    printf("  - Average attempts: %.1f\n\n", avg);
  } else {
    printf("\n");
  }
}

void handle_error(Status status) {
  switch (status) {
  case ERR_INVALID_INPUT:
    printf("Error: Invalid input. Please try again.\n\n");
    break;
  case ERR_INVALID_OPTION:
    printf("Error: Invalid option selection.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_game_session(SessionStats *stats) {
  int secret = generate_secret_number(DEFAULT_MIN, DEFAULT_MAX);

  printf("\nGame Started! Guess the number between %d and %d.\n", DEFAULT_MIN,
         DEFAULT_MAX);
  printf("You have %d attempts.\n", MAX_ATTEMPTS);

  GameResult result = process_game_session(secret);

  stats->games_played++;
  stats->total_attempts += result.attempts;

  if (result.won) {
    stats->games_won++;
    printf("\nCONGRATULATIONS! You won in %d attempts.\n", result.attempts);
    printf("Efficiency Score: %.1f%%\n", result.efficiency);
  } else {
    printf("\nGAME OVER. The number was %d.\n", secret);
  }

  show_stats(stats);
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

GameResult process_game_session(int secret_number) {
  GameResult res = {SUCCESS, FALSE, 0, 0.0};
  int guess;
  int min = DEFAULT_MIN;
  int max = DEFAULT_MAX;

  for (int i = 1; i <= MAX_ATTEMPTS; i++) {
    printf("\nAttempt %d/%d (Range: %d-%d): ", i, MAX_ATTEMPTS, min, max);
    if (read_integer(&guess) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      i--;
      continue;
    }

    res.attempts = i;

    if (guess == secret_number) {
      res.won = TRUE;
      res.efficiency = 100.0 * (1.0 - ((double)(i - 1) / MAX_ATTEMPTS));
      return res;
    }

    if (guess < secret_number) {
      printf("  -> Too LOW! ");
      if (guess > min)
        min = guess + 1;
    } else {
      printf("  -> Too HIGH! ");
      if (guess < max)
        max = guess - 1;
    }
  }

  return res;
}

int generate_secret_number(int min, int max) {
  return min + rand() % (max - min + 1);
}
