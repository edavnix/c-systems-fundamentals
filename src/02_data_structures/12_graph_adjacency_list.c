/*
 ===============================================================================
 Exercise: 12_graph_adjacency_list.c
 Description: Undirected Graph implementation using Adjacency Lists
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Dynamic vertex management (Labels 'A'-'Z')
 - Adjacency List representation (Array of Linked Lists)
 - Breadth-First Search (BFS) for Shortest Path
 - Degree calculation and connectivity checks
 - Dynamic memory management for edges
 ===============================================================================
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define MAX_VERTICES 26
#define MIN_OPTION 1
#define MAX_OPTION 7

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_MEMORY_ALLOCATION,
  ERR_FULL,
  ERR_ALREADY_EXISTS,
  ERR_NOT_FOUND,
  ERR_NO_PATH
} Status;

typedef struct AdjNode {
  int vertex_idx;
  struct AdjNode *next;
} AdjNode;

typedef struct {
  AdjNode *head[MAX_VERTICES];
  char labels[MAX_VERTICES];
  int num_vertices;
  int num_edges;
} Graph;

void show_menu(void);
void handle_error(Status status);
void run_add_vertex(Graph *g);
void run_add_edge(Graph *g);
void run_show_graph(const Graph *g);
void run_bfs_path(Graph *g);
void run_vertex_degree(Graph *g);
void run_clear_graph(Graph *g);

void clear_input_buffer(void);
Status read_integer(int *value);
Status read_char(char *value);

void init_graph(Graph *g);
Status add_vertex(Graph *g, char label);
Status add_edge(Graph *g, char src, char dest);
int get_vertex_index(const Graph *g, char label);
int get_degree(const Graph *g, int vertex_idx);
Status bfs_shortest_path(Graph *g, char start, char end);
void clear_graph(Graph *g);

int main(void) {
  int option = 0;
  Graph g;

  init_graph(&g);

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nExiting. Cleaning up memory...\n");
      clear_graph(&g);
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    switch (option) {
    case 1:
      run_add_vertex(&g);
      break;
    case 2:
      run_add_edge(&g);
      break;
    case 3:
      run_show_graph(&g);
      break;
    case 4:
      run_bfs_path(&g);
      break;
    case 5:
      run_vertex_degree(&g);
      break;
    case 6:
      run_clear_graph(&g);
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Graph (Adjacency List) ===\n\n");
  printf("1. Add Vertex\n2. Add Edge\n3. Show Graph\n"
         "4. Find Path (BFS)\n5. Vertex Degree\n6. Clear Graph\n7. Exit\n");
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
  case ERR_MEMORY_ALLOCATION:
    printf("Error: Memory allocation failed.\n\n");
    break;
  case ERR_FULL:
    printf("Error: Graph is full (Max vertices reached).\n\n");
    break;
  case ERR_ALREADY_EXISTS:
    printf("Error: Vertex or Edge already exists.\n\n");
    break;
  case ERR_NOT_FOUND:
    printf("Error: Vertex not found.\n\n");
    break;
  case ERR_NO_PATH:
    printf("Error: No path found between vertices.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_add_vertex(Graph *g) {
  char label;
  printf("\nVertex Label (A-Z): ");
  if (read_char(&label) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }
  label = toupper(label);

  Status status = add_vertex(g, label);
  if (status == SUCCESS) {
    printf("\n  - Vertex '%c' added.\n\n", label);
  } else {
    handle_error(status);
  }
}

void run_add_edge(Graph *g) {
  char src, dest;
  printf("\nSource Vertex: ");
  if (read_char(&src) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }
  printf("Destination Vertex: ");
  if (read_char(&dest) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  src = toupper(src);
  dest = toupper(dest);

  Status status = add_edge(g, src, dest);
  if (status == SUCCESS) {
    printf("\n  - Edge added: %c <-> %c\n\n", src, dest);
  } else {
    handle_error(status);
  }
}

void run_show_graph(const Graph *g) {
  if (g->num_vertices == 0) {
    printf("\n  - Graph is empty.\n\n");
    return;
  }

  printf("\nAdjacency List:\n");
  for (int i = 0; i < g->num_vertices; i++) {
    printf("  %c -> ", g->labels[i]);
    AdjNode *temp = g->head[i];
    if (!temp) {
      printf("[None]");
    }
    while (temp) {
      printf("[%c] ", g->labels[temp->vertex_idx]);
      temp = temp->next;
    }
    printf("\n");
  }
  printf("\n  - Vertices: %d | Edges: %d\n\n", g->num_vertices, g->num_edges);
}

void run_bfs_path(Graph *g) {
  char start, end;
  printf("\nStart info: ");
  printf("\nStart Vertex: ");
  if (read_char(&start) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }
  printf("Target Vertex: ");
  if (read_char(&end) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  start = toupper(start);
  end = toupper(end);

  Status status = bfs_shortest_path(g, start, end);
  if (status != SUCCESS) {
    handle_error(status);
  }
}

void run_vertex_degree(Graph *g) {
  char label;
  printf("\nVertex to check: ");
  if (read_char(&label) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }
  label = toupper(label);

  int idx = get_vertex_index(g, label);
  if (idx == -1) {
    handle_error(ERR_NOT_FOUND);
    return;
  }

  int degree = get_degree(g, idx);
  printf("\n  - Vertex '%c' has degree: %d\n\n", label, degree);
}

void run_clear_graph(Graph *g) {
  clear_graph(g);
  printf("\n  - Graph cleared.\n\n");
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

void init_graph(Graph *g) {
  g->num_vertices = 0;
  g->num_edges = 0;
  for (int i = 0; i < MAX_VERTICES; i++) {
    g->head[i] = NULL;
  }
}

int get_vertex_index(const Graph *g, char label) {
  for (int i = 0; i < g->num_vertices; i++) {
    if (g->labels[i] == label) {
      return i;
    }
  }

  return -1;
}

Status add_vertex(Graph *g, char label) {
  if (g->num_vertices >= MAX_VERTICES) {
    return ERR_FULL;
  }

  if (get_vertex_index(g, label) != -1) {
    return ERR_ALREADY_EXISTS;
  }

  g->labels[g->num_vertices] = label;
  g->head[g->num_vertices] = NULL;
  g->num_vertices++;

  return SUCCESS;
}

Status add_edge(Graph *g, char src, char dest) {
  int u = get_vertex_index(g, src);
  int v = get_vertex_index(g, dest);

  if (u == -1 || v == -1) {
    return ERR_NOT_FOUND;
  }

  if (u == v) {
    return ERR_INVALID_INPUT;
  }

  AdjNode *temp = g->head[u];
  while (temp) {
    if (temp->vertex_idx == v) {
      return ERR_ALREADY_EXISTS;
    }
    temp = temp->next;
  }

  AdjNode *newNodeUV = (AdjNode *)malloc(sizeof(AdjNode));
  if (!newNodeUV) {
    return ERR_MEMORY_ALLOCATION;
  }
  newNodeUV->vertex_idx = v;
  newNodeUV->next = g->head[u];
  g->head[u] = newNodeUV;

  AdjNode *newNodeVU = (AdjNode *)malloc(sizeof(AdjNode));
  if (!newNodeVU) {
    return ERR_MEMORY_ALLOCATION;
  }
  newNodeVU->vertex_idx = u;
  newNodeVU->next = g->head[v];
  g->head[v] = newNodeVU;

  g->num_edges++;

  return SUCCESS;
}

int get_degree(const Graph *g, int vertex_idx) {
  int degree = 0;
  AdjNode *temp = g->head[vertex_idx];
  while (temp) {
    degree++;
    temp = temp->next;
  }

  return degree;
}

Status bfs_shortest_path(Graph *g, char start, char end) {
  int s = get_vertex_index(g, start);
  int e = get_vertex_index(g, end);

  if (s == -1 || e == -1) {
    return ERR_NOT_FOUND;
  }

  int queue[MAX_VERTICES];
  int front = 0, rear = 0;
  int visited[MAX_VERTICES];
  int parent[MAX_VERTICES];

  for (int i = 0; i < MAX_VERTICES; i++) {
    visited[i] = FALSE;
    parent[i] = -1;
  }

  visited[s] = TRUE;
  queue[rear++] = s;

  int found = FALSE;
  while (front < rear) {
    int u = queue[front++];
    if (u == e) {
      found = TRUE;
      break;
    }

    AdjNode *temp = g->head[u];
    while (temp) {
      int v = temp->vertex_idx;
      if (!visited[v]) {
        visited[v] = TRUE;
        parent[v] = u;
        queue[rear++] = v;
      }
      temp = temp->next;
    }
  }

  if (found) {
    // Reconstruct path
    int path[MAX_VERTICES];
    int path_len = 0;
    int curr = e;
    while (curr != -1) {
      path[path_len++] = curr;
      curr = parent[curr];
    }

    printf("\n  - Path found: ");
    for (int i = path_len - 1; i >= 0; i--) {
      printf("%c", g->labels[path[i]]);
      if (i > 0)
        printf(" -> ");
    }
    printf("\n  - Distance: %d hops\n\n", path_len - 1);
    return SUCCESS;
  } else {
    return ERR_NO_PATH;
  }
}

void clear_graph(Graph *g) {
  for (int i = 0; i < g->num_vertices; i++) {
    AdjNode *current = g->head[i];
    while (current) {
      AdjNode *temp = current;
      current = current->next;
      free(temp);
    }
    g->head[i] = NULL;
  }
  g->num_vertices = 0;
  g->num_edges = 0;
}
