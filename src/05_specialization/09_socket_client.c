/*
 ===============================================================================
 Exercise: 09_socket_client.c
 Description: Basic TCP Client (socket, connect, send, recv)
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - IPv4 TCP Socket creation and active connection
 - Persistent bidirectional communication loop
 - Single-shot message mode for quick tests
 - Safe string handling and terminal input cleaning
 - Statistics tracking for network metrics
 - Interactive menu for connection modes
 ===============================================================================
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_BUFFER 1024
#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 4

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_SOCKET_CREATE,
  ERR_INVALID_IP,
  ERR_CONNECT_FAILED,
  ERR_SEND_FAILED,
  ERR_RECV_FAILED
} Status;

typedef struct {
  int messages_sent;
  int bytes_sent;
  int bytes_received;
} ClientStats;

void show_menu(void);
void handle_error(Status status);

void run_interactive_client(void);
void run_single_shot(void);
void run_client_info(void);

void clear_input_buffer(void);
Status read_integer(int *value);
Status read_string(char *buffer, int max_len);

int create_and_connect(const char *ip, int port);
void print_stats(ClientStats *stats);

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
      run_interactive_client();
      break;
    case 2:
      run_single_shot();
      break;
    case 3:
      run_client_info();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== TCP Socket Client ===\n\n");
  printf("1. Interactive Client (persistent connection)\n");
  printf("2. Single-Shot Message\n");
  printf("3. Client API Reference\n");
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
  case ERR_INVALID_IP:
    printf("Error: Invalid IP address format.\n\n");
    break;
  case ERR_CONNECT_FAILED:
    printf("Error: Connection refused. Is the server running?\n\n");
    break;
  case ERR_SEND_FAILED:
    printf("Error: Failed to send data.\n\n");
    break;
  case ERR_RECV_FAILED:
    printf("Error: Failed to receive data.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

int create_and_connect(const char *ip, int port) {
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    handle_error(ERR_SOCKET_CREATE);
    return -1;
  }
  printf("  - socket() created: fd=%d\n", sock_fd);

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
    handle_error(ERR_INVALID_IP);
    close(sock_fd);
    return -1;
  }

  printf("  - Connecting to %s:%d...\n", ip, port);
  if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    handle_error(ERR_CONNECT_FAILED);
    close(sock_fd);
    return -1;
  }

  printf("  - Connected successfully.\n\n");
  return sock_fd;
}

void print_stats(ClientStats *stats) {
  printf("\n=== Session Statistics ===\n");
  printf("  - Messages Sent:  %d\n", stats->messages_sent);
  printf("  - Bytes Sent:     %d\n", stats->bytes_sent);
  printf("  - Bytes Received: %d\n\n", stats->bytes_received);
}

void run_interactive_client(void) {
  char ip[INET_ADDRSTRLEN];
  int port;

  printf("\nServer IP (e.g. 127.0.0.1): ");
  if (read_string(ip, sizeof(ip)) != SUCCESS || strlen(ip) == 0) {
    strcpy(ip, "127.0.0.1");
  }

  printf("Server Port (e.g. 8080): ");
  if (read_integer(&port) != SUCCESS || port <= 0 || port > 65535) {
    port = 8080;
  }

  printf("\n=== Interactive Client -> %s:%d ===\n", ip, port);

  int sock_fd = create_and_connect(ip, port);
  if (sock_fd < 0)
    return;

  ClientStats stats = {0, 0, 0};
  char buffer[MAX_BUFFER];
  char response[MAX_BUFFER];

  printf("  Type messages to send. Type 'exit' to disconnect.\n\n");

  while (TRUE) {
    printf("  > ");
    if (read_string(buffer, sizeof(buffer)) != SUCCESS) {
      continue;
    }

    if (strcmp(buffer, "exit") == 0 || strlen(buffer) == 0) {
      break;
    }

    int msg_len = strlen(buffer);
    int bytes_sent = send(sock_fd, buffer, msg_len, 0);
    if (bytes_sent < 0) {
      handle_error(ERR_SEND_FAILED);
      break;
    }

    stats.messages_sent++;
    stats.bytes_sent += bytes_sent;
    printf("  [SEND] \"%s\" (%d bytes)\n", buffer, bytes_sent);

    memset(response, 0, MAX_BUFFER);
    int bytes_recv = recv(sock_fd, response, MAX_BUFFER - 1, 0);

    if (bytes_recv < 0) {
      handle_error(ERR_RECV_FAILED);
      break;
    } else if (bytes_recv == 0) {
      printf("  ⚠ Server closed the connection.\n");
      break;
    }

    response[strcspn(response, "\r\n")] = 0;
    stats.bytes_received += bytes_recv;
    printf("  [RECV] \"%s\" (%d bytes)\n\n", response, bytes_recv);
  }

  printf("\n  Disconnecting...\n");
  close(sock_fd);
  print_stats(&stats);
}

void run_single_shot(void) {
  char ip[INET_ADDRSTRLEN];
  char message[MAX_BUFFER];
  int port;

  printf("\nServer IP (e.g. 127.0.0.1): ");
  if (read_string(ip, sizeof(ip)) != SUCCESS || strlen(ip) == 0) {
    strcpy(ip, "127.0.0.1");
  }

  printf("Server Port (e.g. 8080): ");
  if (read_integer(&port) != SUCCESS || port <= 0 || port > 65535) {
    port = 8080;
  }

  printf("Message to send: ");
  if (read_string(message, sizeof(message)) != SUCCESS ||
      strlen(message) == 0) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("\n=== Single-Shot -> %s:%d ===\n", ip, port);

  int sock_fd = create_and_connect(ip, port);
  if (sock_fd < 0)
    return;

  int msg_len = strlen(message);
  int bytes_sent = send(sock_fd, message, msg_len, 0);
  if (bytes_sent < 0) {
    handle_error(ERR_SEND_FAILED);
    close(sock_fd);
    return;
  }
  printf("  [SEND] \"%s\" (%d bytes)\n", message, bytes_sent);

  char response[MAX_BUFFER];
  memset(response, 0, MAX_BUFFER);
  int bytes_recv = recv(sock_fd, response, MAX_BUFFER - 1, 0);

  if (bytes_recv > 0) {
    response[strcspn(response, "\r\n")] = 0;
    printf("  [RECV] \"%s\" (%d bytes)\n", response, bytes_recv);
  } else if (bytes_recv == 0) {
    printf("  - Server closed connection (no response).\n");
  }

  close(sock_fd);
  printf("  - Connection closed.\n\n");
}

void run_client_info(void) {
  printf("\n=== TCP Client API Reference ===\n\n");
  printf("  Client Flow:\n");
  printf("  socket() -> connect() -> send()/recv() -> close()\n\n");

  printf("  %-20s | %s\n", "Function", "Purpose");
  printf(
      "  ---------------------|------------------------------------------\n");
  printf("  %-20s | %s\n", "socket()", "Create client endpoint (fd)");
  printf("  %-20s | %s\n", "inet_pton()", "Convert IP string to binary");
  printf("  %-20s | %s\n", "connect()", "Initiate 3-way TCP handshake");
  printf("  %-20s | %s\n", "send()", "Transmit data to server");
  printf("  %-20s | %s\n", "recv()", "Receive server response");
  printf("  %-20s | %s\n", "close()", "Terminate connection");

  printf("\n  Server vs Client Difference:\n");
  printf("  - Server: socket -> bind -> listen -> accept (passive)\n");
  printf("  - Client: socket -> connect (active)\n\n");

  printf("  Quick Test:\n");
  printf("  Terminal 1: ./socket_server   (start server on 8080)\n");
  printf("  Terminal 2: ./socket_client   (connect to 127.0.0.1:8080)\n\n");
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
