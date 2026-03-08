/*
 ===============================================================================
 Exercise: 08_socket_server.c
 Description: Basic TCP Server (socket, bind, listen, accept)
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Real IPv4 TCP Socket creation
 - Port binding and passive listening
 - Blocking accept() loop for incoming client connections
 - Basic request routing (Time, Echo, Help)
 - Graceful shutdown with statistics on SIGINT (Ctrl+C)
 - Interactive menu for server modes and reference
 ===============================================================================
*/

#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define MAX_BUFFER 1024
#define BACKLOG 5
#define DEFAULT_PORT 8080
#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 4

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_SOCKET_CREATE,
  ERR_SOCKET_BIND,
  ERR_SOCKET_LISTEN
} Status;

/* Server statistics */
typedef struct {
  int connections;
  int messages;
  int bytes_received;
  int bytes_sent;
} ServerStats;

volatile sig_atomic_t keep_running = 1;

void show_menu(void);
void handle_error(Status status);

void run_echo_server(void);
void run_router_server(void);
void run_socket_info(void);

void clear_input_buffer(void);
Status read_integer(int *value);

void on_sigint(int sig);
int setup_server_socket(int port);
void handle_client(int client_fd, struct sockaddr_in *client_addr,
                   ServerStats *stats, int route_mode);
void print_stats(ServerStats *stats);

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
      run_echo_server();
      break;
    case 2:
      run_router_server();
      break;
    case 3:
      run_socket_info();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== TCP Socket Server ===\n\n");
  printf("1. Start Echo Server (echoes back messages)\n");
  printf("2. Start Router Server (TIME, HELP, ECHO commands)\n");
  printf("3. Socket API Reference\n");
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
  case ERR_SOCKET_CREATE:
    printf("Error: Failed to create socket.\n\n");
    break;
  case ERR_SOCKET_BIND:
    printf("Error: Failed to bind socket to port.\n\n");
    break;
  case ERR_SOCKET_LISTEN:
    printf("Error: Failed to listen on socket.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void on_sigint(int sig) { keep_running = 0; }

int setup_server_socket(int port) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    handle_error(ERR_SOCKET_CREATE);
    return -1;
  }
  printf("  - socket() created: fd=%d\n", server_fd);

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    handle_error(ERR_SOCKET_BIND);
    close(server_fd);
    return -1;
  }
  printf("  - bind() successful on 0.0.0.0:%d\n", port);

  if (listen(server_fd, BACKLOG) < 0) {
    handle_error(ERR_SOCKET_LISTEN);
    close(server_fd);
    return -1;
  }
  printf("  - listen() active (backlog: %d)\n\n", BACKLOG);

  return server_fd;
}

void handle_client(int client_fd, struct sockaddr_in *client_addr,
                   ServerStats *stats, int route_mode) {
  char buffer[MAX_BUFFER];
  char response[MAX_BUFFER];

  stats->connections++;

  char client_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &client_addr->sin_addr, client_ip, INET_ADDRSTRLEN);
  int client_port = ntohs(client_addr->sin_port);

  printf("  [ACCEPT] Connection #%d from %s:%d\n", stats->connections,
         client_ip, client_port);

  memset(buffer, 0, MAX_BUFFER);
  int bytes_read = recv(client_fd, buffer, MAX_BUFFER - 1, 0);

  if (bytes_read <= 0) {
    printf("  [CLOSE] Client disconnected without sending data.\n\n");
    close(client_fd);
    return;
  }

  // Clean trailing newlines from telnet/nc
  buffer[strcspn(buffer, "\r\n")] = 0;
  int clean_len = strlen(buffer);
  stats->messages++;
  stats->bytes_received += clean_len;

  printf("  [RECV] \"%s\" (%d bytes)\n", buffer, clean_len);

  memset(response, 0, MAX_BUFFER);

  if (route_mode) {
    // Router mode: parse commands
    if (strcmp(buffer, "TIME") == 0) {
      time_t now = time(NULL);
      struct tm *t = localtime(&now);
      sprintf(response, "Server Time: %02d:%02d:%02d", t->tm_hour, t->tm_min,
              t->tm_sec);
    } else if (strcmp(buffer, "HELP") == 0) {
      sprintf(response, "Commands: TIME, HELP, ECHO <msg>, QUIT");
    } else if (strncmp(buffer, "ECHO ", 5) == 0) {
      sprintf(response, "Echo: %s", buffer + 5);
    } else if (strcmp(buffer, "QUIT") == 0) {
      sprintf(response, "Goodbye!");
    } else {
      sprintf(response, "Unknown command. Send HELP for usage.");
    }
  } else {
    // Echo mode: mirror message back
    sprintf(response, "Echo: %s", buffer);
  }

  int resp_len = strlen(response);
  send(client_fd, response, resp_len, 0);
  send(client_fd, "\n", 1, 0);
  stats->bytes_sent += resp_len;

  printf("  [SEND] \"%s\" (%d bytes)\n", response, resp_len);

  close(client_fd);
  printf("  [CLOSE] Connection closed.\n\n");
}

void print_stats(ServerStats *stats) {
  printf("\n=== Server Statistics ===\n");
  printf("  - Total Connections: %d\n", stats->connections);
  printf("  - Messages Processed: %d\n", stats->messages);
  printf("  - Bytes Received: %d\n", stats->bytes_received);
  printf("  - Bytes Sent: %d\n\n", stats->bytes_sent);
}

void run_echo_server(void) {
  int port;
  printf("\nEnter port (default %d): ", DEFAULT_PORT);
  if (read_integer(&port) != SUCCESS || port <= 0 || port > 65535) {
    port = DEFAULT_PORT;
  }

  printf("\n=== Echo Server (Port %d) ===\n", port);
  printf("  Setting up server...\n");

  // Install SIGINT handler
  struct sigaction sa;
  sa.sa_handler = on_sigint;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
  keep_running = 1;

  int server_fd = setup_server_socket(port);
  if (server_fd < 0)
    return;

  ServerStats stats = {0, 0, 0, 0};

  printf("  Server ready. Test with: nc localhost %d\n", port);
  printf("  Press Ctrl+C to stop.\n\n");

  while (keep_running) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

    if (client_fd < 0) {
      if (errno == EINTR)
        break;
      continue;
    }

    handle_client(client_fd, &client_addr, &stats, FALSE);
  }

  printf("\n  Shutting down Echo Server...\n");
  close(server_fd);
  print_stats(&stats);

  // Restore default SIGINT
  sa.sa_handler = SIG_DFL;
  sigaction(SIGINT, &sa, NULL);
}

void run_router_server(void) {
  int port;
  printf("\nEnter port (default %d): ", DEFAULT_PORT);
  if (read_integer(&port) != SUCCESS || port <= 0 || port > 65535) {
    port = DEFAULT_PORT;
  }

  printf("\n=== Router Server (Port %d) ===\n", port);
  printf("  Commands: TIME, HELP, ECHO <msg>, QUIT\n");
  printf("  Setting up server...\n");

  struct sigaction sa;
  sa.sa_handler = on_sigint;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
  keep_running = 1;

  int server_fd = setup_server_socket(port);
  if (server_fd < 0)
    return;

  ServerStats stats = {0, 0, 0, 0};

  printf("  Server ready. Test with: nc localhost %d\n", port);
  printf("  Press Ctrl+C to stop.\n\n");

  while (keep_running) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

    if (client_fd < 0) {
      if (errno == EINTR)
        break;
      continue;
    }

    handle_client(client_fd, &client_addr, &stats, TRUE);
  }

  printf("\n  Shutting down Router Server...\n");
  close(server_fd);
  print_stats(&stats);

  sa.sa_handler = SIG_DFL;
  sigaction(SIGINT, &sa, NULL);
}

void run_socket_info(void) {
  printf("\n=== Socket API Reference ===\n\n");
  printf("  TCP Server Flow:\n");
  printf("  socket() -> bind() -> listen() -> accept() -> recv/send -> "
         "close()\n\n");

  printf("  %-20s | %s\n", "Function", "Purpose");
  printf("  ---------------------|------------------------------------------\n");
  printf("  %-20s | %s\n", "socket()", "Create endpoint (returns fd)");
  printf("  %-20s | %s\n", "bind()", "Assign address + port to socket");
  printf("  %-20s | %s\n", "listen()", "Mark socket as passive (server)");
  printf("  %-20s | %s\n", "accept()", "Block until client connects");
  printf("  %-20s | %s\n", "recv()", "Read data from connected client");
  printf("  %-20s | %s\n", "send()", "Write data to connected client");
  printf("  %-20s | %s\n", "close()", "Release socket file descriptor");
  printf("  %-20s | %s\n", "setsockopt()", "Set SO_REUSEADDR to avoid bind errors");

  printf("\n  Quick Test:\n");
  printf("  Terminal 1: ./socket_server   (starts server)\n");
  printf("  Terminal 2: nc localhost 8080 (connect as client)\n");
  printf("  Type a message and press Enter.\n\n");
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
