/*
 ===============================================================================
 Exercise: 16_graph_dfs.c
 Description: Depth-First Search (DFS) Visualization
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Graph representation using Adjacency Lists
 - Explicit Stack implementation to visualize LIFO behavior
 - Step-by-step DFS execution log
 - "Demo Mode" to replicate specific tree structures
 - Dynamic memory management with proper cleanup
 ===============================================================================
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define MAX_VERTICES 26
#define MAX_STACK 100
#define MIN_OPTION 1
#define MAX_OPTION 5

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_FULL,
  ERR_NOT_FOUND,
  ERR_ALREADY_EXISTS,
  ERR_MEMORY_ALLOCATION
} Status;

typedef struct AdjNode {
  int vertex_idx;
  struct AdjNode *next;
} AdjNode;

typedef struct {
  AdjNode *head[MAX_VERTICES];
  char labels[MAX_VERTICES];
  int visited[MAX_VERTICES];
  int num_vertices;
} Graph;

typedef struct {
  int items[MAX_STACK];
  int top;
} Stack;

void show_menu(void);
void handle_error(Status status);
void run_load_demo(Graph *g);
void run_custom_graph(Graph *g);
void run_show_graph(Graph *g);
void run_execute_dfs(Graph *g);

void clear_input_buffer(void);
Status read_integer(int *value);
Status read_char(char *value);

void init_graph(Graph *g);
Status add_vertex(Graph *g, char label);
Status add_edge(Graph *g, char src, char dest);
int get_vertex_index(Graph *g, char label);
void reset_visited(Graph *g);
void clear_graph(Graph *g);
void init_stack(Stack *s);
void push(Stack *s, int value);
int pop(Stack *s);
int is_empty(Stack *s);
void print_stack(Stack *s, Graph *g);

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
      printf("\nExiting. Memory cleanup...\n");
      clear_graph(&g);
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    switch (option) {
    case 1:
      run_load_demo(&g);
      break;
    case 2:
      run_custom_graph(&g);
      break;
    case 3:
      run_show_graph(&g);
      break;
    case 4:
      run_execute_dfs(&g);
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Depth-First Search (DFS) Visualizer ===\n\n");
  printf("1. Load Demo Graph (A-F Tree)\n2. Create Custom Graph\n"
         "3. Show Graph Structure\n4. Run DFS Visualization\n5. Exit\n");
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
  case ERR_FULL:
    printf("Error: Graph/Stack is full.\n\n");
    break;
  case ERR_NOT_FOUND:
    printf("Error: Element not found.\n\n");
    break;
  case ERR_ALREADY_EXISTS:
    printf("Error: Element already exists.\n\n");
    break;
  case ERR_MEMORY_ALLOCATION:
    printf("Error: Memory allocation failed.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_load_demo(Graph *g) {
  clear_graph(g);
  init_graph(g);
  printf("\nLoading Demo Graph...\n");

  char verts[] = {'A', 'B', 'C', 'D', 'E', 'F'};
  for (int i = 0; i < 6; i++) {
    add_vertex(g, verts[i]);
  }

  // A -> B, A -> C
  add_edge(g, 'A', 'B');
  add_edge(g, 'A', 'C');

  // B -> D, B -> E
  add_edge(g, 'B', 'D');
  add_edge(g, 'B', 'E');

  // C -> F
  add_edge(g, 'C', 'F');

  printf("  - Demo graph loaded successfully.\n");
  run_show_graph(g);
}

void run_custom_graph(Graph *g) {
  clear_graph(g);
  init_graph(g);
  printf("\n=== Custom Graph Construction ===\n");
  printf("Enter vertices (single char). Enter '.' to stop.\n");

  while (TRUE) {
    char v;
    printf("  - Vertex Label: ");
    if (read_char(&v) != SUCCESS)
      continue;
    if (v == '.')
      break;

    Status st = add_vertex(g, toupper(v));
    if (st == ERR_FULL) {
      handle_error(st);
      break;
    }
    if (st == ERR_ALREADY_EXISTS) {
      printf("    (Vertex already exists)\n");
    }
  }

  printf("\nEnter edges (Source Destination). Enter '.' to stop.\n");
  while (TRUE) {
    char u, v;
    printf("  - Edge (Src Dest): ");
    if (read_char(&u) != SUCCESS)
      continue;
    if (u == '.')
      break;
    if (read_char(&v) != SUCCESS)
      continue;

    Status st = add_edge(g, toupper(u), toupper(v));
    if (st != SUCCESS)
      handle_error(st);
    else
      printf("    Added %c -> %c\n", toupper(u), toupper(v));
  }
  printf("\n  - Custom graph created.\n\n");
}

void run_show_graph(Graph *g) {
  if (g->num_vertices == 0) {
    printf("\n  - Graph is empty.\n\n");
    return;
  }

  printf("\n=== Graph Representation ===\n");

  if (g->num_vertices == 6 && g->head[0] != NULL) {
    int is_demo = TRUE;
    char demo_labels[] = {'A', 'B', 'C', 'D', 'E', 'F'};
    for (int i = 0; i < 6; i++) {
      if (g->labels[i] != demo_labels[i])
        is_demo = FALSE;
    }

    if (is_demo) {
      printf("    A\n");
      printf("   / \\\n");
      printf("  B   C\n");
      printf(" / \\   \\\n");
      printf("D   E   F\n\n");
    }
  }

  printf("Adjacency List:\n");
  for (int i = 0; i < g->num_vertices; i++) {
    printf("  %c -> [", g->labels[i]);
    AdjNode *temp = g->head[i];
    while (temp) {
      printf("%c%s", g->labels[temp->vertex_idx], temp->next ? ", " : "");
      temp = temp->next;
    }
    printf("]\n");
  }
  printf("\n");
}

void run_execute_dfs(Graph *g) {
  if (g->num_vertices == 0) {
    printf("\n  - Graph is empty.\n\n");
    return;
  }

  char start_char;
  printf("\nStart DFS from Vertex: ");
  if (read_char(&start_char) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  int start_idx = get_vertex_index(g, toupper(start_char));
  if (start_idx == -1) {
    handle_error(ERR_NOT_FOUND);
    return;
  }

  reset_visited(g);
  Stack s;
  init_stack(&s);

  printf("\n=== DFS Execution Log ===\n");

  push(&s, start_idx);

  int visit_order[MAX_VERTICES];
  int visit_count = 0;

  while (!is_empty(&s)) {
    print_stack(&s, g);

    int current = pop(&s);

    if (!g->visited[current]) {
      g->visited[current] = TRUE;
      printf("  > Visiting: %c\n", g->labels[current]);
      visit_order[visit_count++] = current;

      AdjNode *temp = g->head[current];
      while (temp != NULL) {
        if (!g->visited[temp->vertex_idx]) {
          push(&s, temp->vertex_idx);
        }
        temp = temp->next;
      }
    }
  }

  printf("  > Stack: [] (Empty)\n");
  printf("\nTraversal Order: ");
  for (int i = 0; i < visit_count; i++) {
    printf("%c%s", g->labels[visit_order[i]],
           (i < visit_count - 1) ? " -> " : "");
  }
  printf("\n\n");
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
  for (int i = 0; i < MAX_VERTICES; i++) {
    g->head[i] = NULL;
    g->visited[i] = FALSE;
  }
}

Status add_vertex(Graph *g, char label) {
  if (g->num_vertices >= MAX_VERTICES) {
    return ERR_FULL;
  }
  if (get_vertex_index(g, label) != -1) {
    return ERR_ALREADY_EXISTS;
  }

  g->labels[g->num_vertices] = label;
  g->num_vertices++;
  return SUCCESS;
}

Status add_edge(Graph *g, char src, char dest) {
  int u = get_vertex_index(g, src);
  int v = get_vertex_index(g, dest);
  if (u == -1 || v == -1) {
    return ERR_NOT_FOUND;
  }

  AdjNode *newNode = (AdjNode *)malloc(sizeof(AdjNode));
  if (!newNode) {
    return ERR_MEMORY_ALLOCATION;
  }

  newNode->vertex_idx = v;
  newNode->next = g->head[u];
  g->head[u] = newNode;

  return SUCCESS;
}

int get_vertex_index(Graph *g, char label) {
  for (int i = 0; i < g->num_vertices; i++) {
    if (g->labels[i] == label) {
      return i;
    }
  }
  return -1;
}

void reset_visited(Graph *g) {
  for (int i = 0; i < MAX_VERTICES; i++) {
    g->visited[i] = FALSE;
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
}

void init_stack(Stack *s) { s->top = -1; }

void push(Stack *s, int value) {
  if (s->top < MAX_STACK - 1) {
    s->items[++s->top] = value;
  }
}

int pop(Stack *s) {
  if (s->top >= 0) {
    return s->items[s->top--];
  }
  return -1;
}

int is_empty(Stack *s) { return s->top == -1; }

void print_stack(Stack *s, Graph *g) {
  printf("  > Stack: [");
  for (int i = 0; i <= s->top; i++) {
    printf("%c%s", g->labels[s->items[i]], (i < s->top) ? ", " : "");
  }
  printf("]\n");
}
