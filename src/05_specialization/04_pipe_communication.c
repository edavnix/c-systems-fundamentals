/*
 ===============================================================================
 Exercise: 04_pipe_communication.c
 Description: Inter-Process Communication (IPC) using Pipes
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Unidirectional pipe (Parent → Child)
 - Bidirectional pipe (Parent ↔ Child using two pipes)
 - File descriptor management (closing unused ends)
 - Data processing in child process (uppercase transform)
 - Interactive menu for multiple demos
 ===============================================================================
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_BUFFER 256
#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 4

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_PIPE_FAILED,
  ERR_FORK_FAILED
} Status;

void show_menu(void);
void handle_error(Status status);

void run_unidirectional_pipe(void);
void run_bidirectional_pipe(void);
void run_pipe_diagram(void);

void clear_input_buffer(void);
Status read_integer(int *value);
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
      run_unidirectional_pipe();
      break;
    case 2:
      run_bidirectional_pipe();
      break;
    case 3:
      run_pipe_diagram();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== IPC Pipe Communication ===\n\n");
  printf("1. Unidirectional Pipe (Parent -> Child)\n");
  printf("2. Bidirectional Pipe (Parent <-> Child)\n");
  printf("3. Pipe Diagram & Concepts\n");
  printf("4. Exit\n");
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
  case ERR_PIPE_FAILED:
    printf("Error: pipe() system call failed.\n\n");
    break;
  case ERR_FORK_FAILED:
    printf("Error: fork() system call failed.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_unidirectional_pipe(void) {
  int fd[2]; // fd[0] = read end, fd[1] = write end
  char buffer[MAX_BUFFER];

  printf("\n=== 1. Unidirectional Pipe ===\n");
  printf("  Direction: Parent (writer) -> Child (reader)\n\n");

  if (pipe(fd) == -1) {
    handle_error(ERR_PIPE_FAILED);
    return;
  }

  printf("  - Pipe created: fd[0]=%d (read), fd[1]=%d (write)\n", fd[0], fd[1]);

  printf("  Enter message to send to child:\n  > ");
  if (read_string(buffer, sizeof(buffer)) != SUCCESS) {
    close(fd[0]);
    close(fd[1]);
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  pid_t pid = fork();

  if (pid < 0) {
    close(fd[0]);
    close(fd[1]);
    handle_error(ERR_FORK_FAILED);
    return;
  } else if (pid > 0) {
    // Parent: Writer
    close(fd[0]); // Close unused read end

    int bytes = strlen(buffer) + 1;
    write(fd[1], buffer, bytes);
    printf("  [PARENT PID:%d] Sent: \"%s\" (%d bytes)\n", getpid(), buffer,
           bytes);

    close(fd[1]); // Close write end (sends EOF to reader)
    wait(NULL);

    printf("  [PARENT PID:%d] Child finished.\n\n", getpid());
  } else {
    // Child: Reader
    close(fd[1]); // Close unused write end

    char recv_buf[MAX_BUFFER];
    int bytes_read = read(fd[0], recv_buf, sizeof(recv_buf));

    if (bytes_read > 0) {
      printf("  [CHILD  PID:%d] Received: \"%s\" (%d bytes)\n", getpid(),
             recv_buf, bytes_read);

      // Process: convert to uppercase
      for (int i = 0; recv_buf[i] != '\0'; i++) {
        recv_buf[i] = toupper(recv_buf[i]);
      }
      printf("  [CHILD  PID:%d] Processed (UPPER): \"%s\"\n", getpid(),
             recv_buf);
    }

    close(fd[0]);
    exit(0);
  }
}

void run_bidirectional_pipe(void) {
  int pipe_parent_to_child[2]; // Parent writes, Child reads
  int pipe_child_to_parent[2]; // Child writes, Parent reads
  char buffer[MAX_BUFFER];

  printf("\n=== 2. Bidirectional Pipe ===\n");
  printf("  Direction: Parent <-> Child (requires two pipes)\n\n");

  if (pipe(pipe_parent_to_child) == -1 || pipe(pipe_child_to_parent) == -1) {
    handle_error(ERR_PIPE_FAILED);
    return;
  }

  printf("  - Pipe1 (P->C): fd[0]=%d, fd[1]=%d\n", pipe_parent_to_child[0],
         pipe_parent_to_child[1]);
  printf("  - Pipe2 (C->P): fd[0]=%d, fd[1]=%d\n\n", pipe_child_to_parent[0],
         pipe_child_to_parent[1]);

  printf("  Enter message to send to child:\n  > ");
  if (read_string(buffer, sizeof(buffer)) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  pid_t pid = fork();

  if (pid < 0) {
    handle_error(ERR_FORK_FAILED);
    return;
  } else if (pid > 0) {
    // Parent
    close(pipe_parent_to_child[0]); // Close read end of P->C
    close(pipe_child_to_parent[1]); // Close write end of C->P

    // Send message to child
    int bytes = strlen(buffer) + 1;
    write(pipe_parent_to_child[1], buffer, bytes);
    printf("  [PARENT PID:%d] Sent to child: \"%s\"\n", getpid(), buffer);
    close(pipe_parent_to_child[1]);

    // Read response from child
    char response[MAX_BUFFER];
    int bytes_read = read(pipe_child_to_parent[0], response, sizeof(response));
    if (bytes_read > 0) {
      printf("  [PARENT PID:%d] Received from child: \"%s\"\n", getpid(),
             response);
    }
    close(pipe_child_to_parent[0]);

    wait(NULL);
    printf("  [PARENT PID:%d] Bidirectional exchange complete.\n\n", getpid());
  } else {
    // Child
    close(pipe_parent_to_child[1]); // Close write end of P->C
    close(pipe_child_to_parent[0]); // Close read end of C->P

    // Read from parent
    char recv_buf[MAX_BUFFER];
    int bytes_read = read(pipe_parent_to_child[0], recv_buf, sizeof(recv_buf));
    close(pipe_parent_to_child[0]);

    if (bytes_read > 0) {
      printf("  [CHILD  PID:%d] Received from parent: \"%s\"\n", getpid(),
             recv_buf);

      // Process: reverse the string
      int len = strlen(recv_buf);
      char reversed[MAX_BUFFER];
      for (int i = 0; i < len; i++) {
        reversed[i] = recv_buf[len - 1 - i];
      }
      reversed[len] = '\0';

      printf("  [CHILD  PID:%d] Sending back reversed: \"%s\"\n", getpid(),
             reversed);

      // Send result back to parent
      write(pipe_child_to_parent[1], reversed, strlen(reversed) + 1);
    }

    close(pipe_child_to_parent[1]);
    exit(0);
  }
}

void run_pipe_diagram(void) {
  printf("\n=== 3. Pipe Concepts ===\n\n");

  printf("  Unidirectional Pipe:\n");
  printf("  PARENT --write()--> | PIPE (kernel buffer) | --read()--> CHILD\n");
  printf("          fd[1]                                  fd[0]\n\n");

  printf("  Bidirectional (Two Pipes):\n");
  printf("  PARENT --write()--> | PIPE 1 | --read()--> CHILD\n");
  printf("  PARENT <--read()--- | PIPE 2 | <--write()- CHILD\n\n");

  printf("  Key Rules:\n");
  printf("  - pipe() must be called BEFORE fork().\n");
  printf("  - Close unused ends to avoid deadlocks.\n");
  printf("  - read() blocks until data is available or write end is closed.\n");
  printf("  - Closing write end sends EOF to reader.\n");
  printf("  - Pipes are unidirectional; use two for bidirectional.\n\n");
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
