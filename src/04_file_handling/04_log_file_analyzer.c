/*
 ===============================================================================
 Exercise: 04_log_file_analyzer.c
 Description: Log File Analyzer with Temporal & Keyword Analysis
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Log parsing and level distribution (INFO, WARNING, ERROR, CRITICAL)
 - ASCII Progress bars for visual reporting
 - Keyword context filtering
 - Hourly temporal peak detection
 - Dummy log file generation for testing
 ===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_FILENAME "system.log"
#define MAX_LINE 256
#define MAX_MSG 150
#define TOTAL_HOURS 24
#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 6

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_FILE_NOT_FOUND,
  ERR_PARSE_ERROR
} Status;

typedef struct {
  char timestamp[25]; // [YYYY-MM-DD HH:MM:SS]
  int hour;
  char level[15];
  char message[MAX_MSG];
} LogEntry;

void show_menu(void);
void handle_error(Status status);

void run_show_summary(void);
void run_filter_by_level(void);
void run_search_keyword(void);
void run_temporal_analysis(void);
void run_generate_dummy_log(void);

void clear_input_buffer(void);
Status read_integer(int *value);
Status read_string(char *buffer, int max_len);

Status parse_log_line(char *line, LogEntry *entry);
void draw_bar(int count, int max_count, int max_bar_len);

int main(void) {
  int option = 0;

  // Generate a dummy log file to work with if it doesn't exist
  run_generate_dummy_log();

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
      run_show_summary();
      break;
    case 2:
      run_filter_by_level();
      break;
    case 3:
      run_search_keyword();
      break;
    case 4:
      run_temporal_analysis();
      break;
    case 5:
      // Regenerate fresh dummy log
      remove(LOG_FILENAME);
      run_generate_dummy_log();
      printf("\n  - New dummy log file generated.\n\n");
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== System Log Analyzer ===\n\n");
  printf("Target: %s\n\n", LOG_FILENAME);
  printf("1. Overall Summary (Level Distribution)\n");
  printf("2. Filter by Level\n");
  printf("3. Keyword Context Search\n");
  printf("4. Temporal Analysis (Hourly)\n");
  printf("5. Regenerate Dummy Log\n");
  printf("6. Exit\n");
  printf("Option: ");
}

void handle_error(Status status) {
  switch (status) {
  case ERR_INVALID_INPUT:
    printf("Error: Invalid input.\n\n");
    break;
  case ERR_INVALID_OPTION:
    printf("Error: Invalid option selected.\n\n");
    break;
  case ERR_FILE_NOT_FOUND:
    printf("Error: Log file '%s' not found.\n\n", LOG_FILENAME);
    break;
  case ERR_PARSE_ERROR:
    printf("Error: Failed to parse log entry format.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

Status parse_log_line(char *line, LogEntry *entry) {
  // Expected format: [2026-01-03 14:23:01] ERROR: Message text

  if (line[0] != '[')
    return ERR_PARSE_ERROR;

  char *end_bracket = strchr(line, ']');
  if (!end_bracket)
    return ERR_PARSE_ERROR;

  // Extract timestamp length
  int ts_len = end_bracket - line + 1;
  if (ts_len >= (int)sizeof(entry->timestamp))
    ts_len = sizeof(entry->timestamp) - 1;
  strncpy(entry->timestamp, line, ts_len);
  entry->timestamp[ts_len] = '\0';

  // Extract hour from timestamp
  char hour_str[3] = {0};
  if (strlen(entry->timestamp) > 13) {
    // [YYYY-MM-DD HH:...
    hour_str[0] = entry->timestamp[12];
    hour_str[1] = entry->timestamp[13];
    entry->hour = atoi(hour_str);
  } else {
    entry->hour = 0;
  }

  // Skip bracket and space
  char *ptr = end_bracket + 2;
  if (*ptr == '\0')
    return ERR_PARSE_ERROR;

  // Find colon separator for Level
  char *colon = strchr(ptr, ':');
  if (!colon)
    return ERR_PARSE_ERROR;

  int level_len = colon - ptr;
  if (level_len >= (int)sizeof(entry->level))
    level_len = sizeof(entry->level) - 1;
  strncpy(entry->level, ptr, level_len);
  entry->level[level_len] = '\0';

  // The rest is the message
  ptr = colon + 2; // Skip colon and space
  strncpy(entry->message, ptr, sizeof(entry->message) - 1);
  entry->message[sizeof(entry->message) - 1] = '\0';

  // Remove trailing newline
  entry->message[strcspn(entry->message, "\n")] = 0;

  return SUCCESS;
}

void draw_bar(int count, int max_count, int max_bar_len) {
  if (max_count == 0)
    return;
  int blocks = (int)(((double)count / max_count) * max_bar_len);
  if (count > 0 && blocks == 0)
    blocks = 1; // Show at least one for small > 0

  for (int i = 0; i < blocks; i++) {
    printf("█");
  }
}

void run_show_summary(void) {
  FILE *file = fopen(LOG_FILENAME, "r");
  if (!file) {
    handle_error(ERR_FILE_NOT_FOUND);
    return;
  }

  char buffer[MAX_LINE];
  LogEntry entry;

  int total = 0, info = 0, warn = 0, err = 0, crit = 0;

  while (fgets(buffer, sizeof(buffer), file)) {
    if (parse_log_line(buffer, &entry) == SUCCESS) {
      total++;
      if (strcmp(entry.level, "INFO") == 0)
        info++;
      else if (strcmp(entry.level, "WARNING") == 0)
        warn++;
      else if (strcmp(entry.level, "ERROR") == 0)
        err++;
      else if (strcmp(entry.level, "CRITICAL") == 0)
        crit++;
    }
  }
  fclose(file);

  if (total == 0) {
    printf("\n  - No valid log entries found.\n\n");
    return;
  }

  // Find max for scaling
  int max_val = info;
  if (warn > max_val)
    max_val = warn;
  if (err > max_val)
    max_val = err;
  if (crit > max_val)
    max_val = crit;

  printf("\n=== Log Summary ===\n");
  printf("  - Total entries processed: %d\n\n", total);
  printf("  Distribution by Level:\n");

  printf("  %-10s: %4d (%4.1f%%) | ", "INFO", info, (double)info / total * 100);
  draw_bar(info, max_val, 30);
  printf("\n");

  printf("  %-10s: %4d (%4.1f%%) | ", "WARNING", warn,
         (double)warn / total * 100);
  draw_bar(warn, max_val, 30);
  printf("\n");

  printf("  %-10s: %4d (%4.1f%%) | ", "ERROR", err, (double)err / total * 100);
  draw_bar(err, max_val, 30);
  printf("\n");

  printf("  %-10s: %4d (%4.1f%%) | ", "CRITICAL", crit,
         (double)crit / total * 100);
  draw_bar(crit, max_val, 30);
  printf("\n\n");
}

void run_filter_by_level(void) {
  char level_query[20];
  printf("\nEnter log level to filter (INFO, WARNING, ERROR, CRITICAL):\n> ");
  if (read_string(level_query, sizeof(level_query)) != SUCCESS)
    return;

  FILE *file = fopen(LOG_FILENAME, "r");
  if (!file) {
    handle_error(ERR_FILE_NOT_FOUND);
    return;
  }

  char buffer[MAX_LINE];
  LogEntry entry;
  int count = 0;
  int limit = 10; // Only show first 10

  printf("\n=== Filtering by '%s' (Showing max %d) ===\n", level_query, limit);

  while (fgets(buffer, sizeof(buffer), file)) {
    if (parse_log_line(buffer, &entry) == SUCCESS) {
      if (strcasecmp(entry.level, level_query) == 0) {
        if (count < limit) {
          printf("  %s %s\n", entry.timestamp, entry.message);
        }
        count++;
      }
    }
  }
  fclose(file);

  printf("\n  - Found %d total entries for level '%s'.\n\n", count,
         level_query);
}

void run_search_keyword(void) {
  char keyword[50];
  printf("\nEnter keyword to search in messages:\n> ");
  if (read_string(keyword, sizeof(keyword)) != SUCCESS || strlen(keyword) == 0)
    return;

  FILE *file = fopen(LOG_FILENAME, "r");
  if (!file) {
    handle_error(ERR_FILE_NOT_FOUND);
    return;
  }

  char buffer[MAX_LINE];
  LogEntry entry;
  int count = 0;
  int limit = 10;

  printf("\n=== Context Search: \"%s\" (Showing max %d) ===\n", keyword, limit);

  while (fgets(buffer, sizeof(buffer), file)) {
    if (parse_log_line(buffer, &entry) == SUCCESS) {
      if (strstr(entry.message, keyword) != NULL) {
        if (count < limit) {
          printf("  %s [%s]: %s\n", entry.timestamp, entry.level,
                 entry.message);
        }
        count++;
      }
    }
  }
  fclose(file);

  printf("\n  - Total keyword matches found: %d\n\n", count);
}

void run_temporal_analysis(void) {
  FILE *file = fopen(LOG_FILENAME, "r");
  if (!file) {
    handle_error(ERR_FILE_NOT_FOUND);
    return;
  }

  char buffer[MAX_LINE];
  LogEntry entry;

  int hourly_counts[TOTAL_HOURS] = {0};
  int max_count = 0;
  int peak_hour = 0;

  while (fgets(buffer, sizeof(buffer), file)) {
    if (parse_log_line(buffer, &entry) == SUCCESS) {
      // Only count anomalies for temporal analysis (ERROR, CRITICAL, WARNING)
      if (strcmp(entry.level, "INFO") != 0) {
        if (entry.hour >= 0 && entry.hour < TOTAL_HOURS) {
          hourly_counts[entry.hour]++;
        }
      }
    }
  }
  fclose(file);

  for (int i = 0; i < TOTAL_HOURS; i++) {
    if (hourly_counts[i] > max_count) {
      max_count = hourly_counts[i];
      peak_hour = i;
    }
  }

  if (max_count == 0) {
    printf("\n  - No anomalies (Warnings/Errors/Critical) found to chart.\n\n");
    return;
  }

  printf("\n=== Temporal Analysis (Anomalies by Hour) ===\n\n");

  for (int i = 0; i < TOTAL_HOURS; i++) {
    if (hourly_counts[i] == 0)
      continue; // Skip zero hours for brevity

    printf("  %02d:00 - %02d:59 : %4d | ", i, i, hourly_counts[i]);
    draw_bar(hourly_counts[i], max_count, 40);

    if (i == peak_hour)
      printf(" ⚠ PEAK");
    printf("\n");
  }

  printf("\n  - Recommendation: Review logs between %02d:00 and %02d:59\n",
         peak_hour, peak_hour);
  printf("  - Highest anomaly activity detected in this range.\n\n");
}

void run_generate_dummy_log(void) {
  FILE *check = fopen(LOG_FILENAME, "r");
  if (check) {
    fclose(check);
    return; // Already exists
  }

  FILE *file = fopen(LOG_FILENAME, "w");
  if (!file)
    return;

  // Generate some realistic looking log data
  const char *levels[] = {"INFO",    "INFO",    "INFO",  "INFO",
                          "WARNING", "WARNING", "ERROR", "CRITICAL"};

  fprintf(file, "[2026-01-03 00:05:12] INFO: System startup initiated\n");
  fprintf(file, "[2026-01-03 00:06:45] INFO: Services loaded successfully\n");

  // Random generation
  for (int i = 0; i < 300; i++) {
    int hour = rand() % 24;
    int min = rand() % 60;
    int sec = rand() % 60;

    // Force a peak cluster at 14:00
    if (i > 150 && i < 200) {
      hour = 14;
    }

    int lvl_idx = rand() % 8;
    const char *lvl = levels[lvl_idx];

    const char *msg;
    if (strcmp(lvl, "INFO") == 0) {
      msg = "User session created normally";
    } else if (strcmp(lvl, "WARNING") == 0) {
      msg = "High memory usage detected (85%)";
    } else if (strcmp(lvl, "ERROR") == 0) {
      int err_type = rand() % 3;
      if (err_type == 0)
        msg = "Database connection timeout";
      else if (err_type == 1)
        msg = "Failed to write to disk";
      else
        msg = "Corrupted data packet received";
    } else {
      msg = "Kernel panic - syncing VFS";
    }

    fprintf(file, "[2026-01-03 %02d:%02d:%02d] %s: %s\n", hour, min, sec, lvl,
            msg);
  }

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
  }
  return SUCCESS;
}
