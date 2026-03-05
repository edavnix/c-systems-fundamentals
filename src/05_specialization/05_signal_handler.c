/*
 ===============================================================================
 Exercise: 05_signal_handler.c
 Description: Advanced Signal Handling (SIGINT, SIGTERM, SIGUSR1, SIGALRM)
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Safe signal handling using sigaction()
 - Volatile sig_atomic_t flags for async-signal safety
 - Graceful degradation and resource cleanup
 - Self-signaling demo using raise()/kill()
 - Timer demo using SIGALRM and alarm()
 - Interactive menu for multiple demos
 ===============================================================================
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 5

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_SIGNAL_SETUP
} Status;

/* Async-signal-safe global flags */
volatile sig_atomic_t got_sigint = 0;
volatile sig_atomic_t got_sigterm = 0;
volatile sig_atomic_t got_sigusr1 = 0;
volatile sig_atomic_t got_sigalrm = 0;

/* Signal Handlers (minimal, only set flags) */
void on_sigint(int sig) { got_sigint = 1; }
void on_sigterm(int sig) { got_sigterm = 1; }
void on_sigusr1(int sig) { got_sigusr1 = 1; }
void on_sigalrm(int sig) { got_sigalrm = 1; }

void show_menu(void);
void handle_error(Status status);

void run_signal_monitor(void);
void run_self_signal_demo(void);
void run_alarm_demo(void);
void run_signal_info(void);

void clear_input_buffer(void);
Status read_integer(int *value);

Status install_handler(int signum, void (*handler)(int), const char *name);

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
      run_signal_monitor();
      break;
    case 2:
      run_self_signal_demo();
      break;
    case 3:
      run_alarm_demo();
      break;
    case 4:
      run_signal_info();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Signal Handler Demos ===\n\n");
  printf("PID: %d (use for 'kill -SIGUSR1 %d')\n\n", getpid(), getpid());
  printf("1. Start Signal Monitor (Ctrl+C to interact)\n");
  printf("2. Self-Signal Demo (raise/kill)\n");
  printf("3. Alarm Timer Demo (SIGALRM)\n");
  printf("4. Signal Concepts & Reference\n");
  printf("5. Exit\n");
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
  case ERR_SIGNAL_SETUP:
    printf("Error: Failed to install signal handler.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

Status install_handler(int signum, void (*handler)(int), const char *name) {
  struct sigaction sa;
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction(signum, &sa, NULL) == -1) {
    printf("  - Failed to install handler for %s.\n", name);
    return ERR_SIGNAL_SETUP;
  }
  printf("  - Installed handler for %s.\n", name);
  return SUCCESS;
}

void run_signal_monitor(void) {
  printf("\n=== 1. Signal Monitor ===\n");
  printf("  PID: %d\n", getpid());
  printf("  Installing signal handlers...\n");

  if (install_handler(SIGINT, on_sigint, "SIGINT (Ctrl+C)") != SUCCESS)
    return;
  if (install_handler(SIGTERM, on_sigterm, "SIGTERM (kill)") != SUCCESS)
    return;
  if (install_handler(SIGUSR1, on_sigusr1, "SIGUSR1 (custom)") != SUCCESS)
    return;

  int sigint_count = 0, sigusr1_count = 0, total_count = 0;

  printf("\n  Waiting for signals... (Ctrl+C to interact, 'kill -SIGUSR1 %d' "
         "from another terminal)\n",
         getpid());

  while (TRUE) {
    pause(); // Suspend until a signal arrives

    if (got_sigint) {
      got_sigint = 0;
      sigint_count++;
      total_count++;

      printf("\n  [SIGNAL] SIGINT received (#%d)\n", sigint_count);
      printf("  Exit? (y/n): ");

      char c = getchar();
      if (c != '\n' && c != EOF)
        clear_input_buffer();

      if (c == 'y' || c == 'Y') {
        printf("\n  === Signal Summary ===\n");
        printf("  - SIGINT:  %d\n", sigint_count);
        printf("  - SIGUSR1: %d\n", sigusr1_count);
        printf("  - Total:   %d signals handled.\n\n", total_count);
        return; // Return to main menu instead of exit()
      }
      printf("  Resuming monitor...\n");
    }

    if (got_sigusr1) {
      got_sigusr1 = 0;
      sigusr1_count++;
      total_count++;
      printf("\n  [SIGNAL] SIGUSR1 received (#%d) - Custom action triggered.\n",
             sigusr1_count);
    }

    if (got_sigterm) {
      got_sigterm = 0;
      total_count++;
      printf("\n  [SIGNAL] SIGTERM received - Graceful shutdown.\n");
      printf("  === Signal Summary ===\n");
      printf("  - SIGINT:  %d\n", sigint_count);
      printf("  - SIGUSR1: %d\n", sigusr1_count);
      printf("  - Total:   %d signals handled.\n\n", total_count);
      return;
    }
  }
}

void run_self_signal_demo(void) {
  printf("\n=== 2. Self-Signal Demo ===\n");
  printf("  PID: %d\n\n", getpid());

  // Install SIGUSR1 handler
  if (install_handler(SIGUSR1, on_sigusr1, "SIGUSR1") != SUCCESS)
    return;

  // Reset flag
  got_sigusr1 = 0;

  printf("  Sending SIGUSR1 to self using raise()...\n");
  raise(SIGUSR1);

  if (got_sigusr1) {
    printf("  - Handler was invoked! Flag = %d\n", got_sigusr1);
    got_sigusr1 = 0;
  }

  printf("\n  Sending SIGUSR1 to self using kill(getpid(), SIGUSR1)...\n");
  kill(getpid(), SIGUSR1);

  if (got_sigusr1) {
    printf("  - Handler was invoked! Flag = %d\n", got_sigusr1);
    got_sigusr1 = 0;
  }

  printf("\n  Both raise() and kill() successfully delivered signals.\n\n");
}

void run_alarm_demo(void) {
  printf("\n=== 3. Alarm Timer Demo (SIGALRM) ===\n");
  printf("  PID: %d\n\n", getpid());

  if (install_handler(SIGALRM, on_sigalrm, "SIGALRM") != SUCCESS)
    return;

  got_sigalrm = 0;

  int seconds = 3;
  printf("  Setting alarm for %d seconds...\n", seconds);
  printf("  Process will block on pause() until alarm fires.\n\n");

  alarm(seconds);

  printf("  Waiting");
  fflush(stdout);

  // Show progress dots while waiting
  for (int i = 0; i < seconds; i++) {
    sleep(1);
    printf(".");
    fflush(stdout);
  }

  // In case alarm already fired during our sleep loop
  if (!got_sigalrm) {
    pause(); // Wait for SIGALRM if not yet received
  }

  if (got_sigalrm) {
    printf("\n\n  [SIGNAL] SIGALRM received after %d seconds!\n", seconds);
    printf("  - Use case: Timeouts, periodic tasks, watchdog timers.\n\n");
    got_sigalrm = 0;
  }
}

void run_signal_info(void) {
  printf("\n=== 4. Signal Concepts ===\n\n");
  printf("  Common Signals:\n");
  printf("  %-10s | %-6s | %s\n", "Signal", "Number", "Description");
  printf("  -----------|--------|----------------------------\n");
  printf("  %-10s | %-6d | %s\n", "SIGINT", SIGINT, "Interrupt (Ctrl+C)");
  printf("  %-10s | %-6d | %s\n", "SIGTERM", SIGTERM,
         "Termination (kill default)");
  printf("  %-10s | %-6d | %s\n", "SIGUSR1", SIGUSR1,
         "User-defined signal 1");
  printf("  %-10s | %-6d | %s\n", "SIGUSR2", SIGUSR2,
         "User-defined signal 2");
  printf("  %-10s | %-6d | %s\n", "SIGALRM", SIGALRM, "Alarm timer expired");
  printf("  %-10s | %-6d | %s\n", "SIGKILL", SIGKILL,
         "Force kill (UNCATCHABLE)");
  printf("  %-10s | %-6d | %s\n", "SIGSTOP", SIGSTOP,
         "Pause process (UNCATCHABLE)");

  printf("\n  Best Practices:\n");
  printf("  - Use sigaction() over signal() (portable and predictable).\n");
  printf("  - Only use volatile sig_atomic_t in handlers.\n");
  printf("  - Keep handlers minimal: set a flag, process in main loop.\n");
  printf("  - Always clean up resources on SIGTERM.\n");
  printf("  - SIGKILL and SIGSTOP cannot be caught or ignored.\n\n");
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
