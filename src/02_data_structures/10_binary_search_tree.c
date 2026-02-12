/*
 ===============================================================================
 Exercise: 10_binary_search_tree.c
 Description: Binary Search Tree (BST) with comprehensive operations
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Recursive Insert, Search, and Delete
 - Deletion logic handling 3 cases (Leaf, One Child, Two Children)
 - Traversals: Inorder, Preorder, Postorder, Level Order (BFS)
 - Tree Properties: Height, Node Count, Balanced Check
 - ASCII Visualization (2D layout logic)
 - Dynamic memory management with proper cleanup
 ===============================================================================
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define MIN_OPTION 1
#define MAX_OPTION 11
#define MAX_QUEUE_SIZE 100

typedef enum {
  SUCCESS,
  ERR_INVALID_INPUT,
  ERR_INVALID_OPTION,
  ERR_MEMORY_ALLOCATION,
  ERR_TREE_EMPTY,
  ERR_DUPLICATE_VAL,
  ERR_NOT_FOUND
} Status;

typedef struct TreeNode {
  int value;
  struct TreeNode *left;
  struct TreeNode *right;
} TreeNode;

void show_menu(void);
void handle_error(Status status);
void run_insert(TreeNode **root);
void run_search(TreeNode *root);
void run_delete(TreeNode **root);
void run_traversals(TreeNode *root, int type);
void run_properties(TreeNode *root);
void run_count(TreeNode *root);
void run_visualize(TreeNode *root);

void clear_input_buffer(void);
Status read_integer(int *value);

Status insert_node(TreeNode **root, int value);
Status delete_node(TreeNode **root, int value);
TreeNode *search_node(TreeNode *root, int value);
TreeNode *find_min(TreeNode *root);
void free_tree(TreeNode *root);
int get_height(TreeNode *root);
int count_nodes(TreeNode *root);
int is_balanced(TreeNode *root);
void print_inorder(TreeNode *root);
void print_preorder(TreeNode *root);
void print_postorder(TreeNode *root);
void print_level_order(TreeNode *root);
void print_tree_structure(TreeNode *root, int space);

int main(void) {
  int option = 0;
  TreeNode *root = NULL;

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nExiting. Freeing memory...\n");
      free_tree(root);
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    switch (option) {
    case 1:
      run_insert(&root);
      break;
    case 2:
      run_search(root);
      break;
    case 3:
      run_delete(&root);
      break;
    case 4:
      run_traversals(root, 1);
      break;
    case 5:
      run_traversals(root, 2);
      break;
    case 6:
      run_traversals(root, 3);
      break;
    case 7:
      run_traversals(root, 4);
      break;
    case 8:
      run_properties(root);
      break;
    case 9:
      run_count(root);
      break;
    case 10:
      run_visualize(root);
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("=== Binary Search Tree (BST) ===\n\n");
  printf("1. Insert node\n2. Search node\n3. Delete node\n"
         "4. Traversal: Inorder (LNR)\n5. Traversal: Preorder (NLR)\n"
         "6. Traversal: Postorder (LRN)\n7. Traversal: Level Order (BFS)\n"
         "8. Tree Properties (Height/Balance)\n9. Count nodes\n"
         "10. Visualize Tree\n11. Exit\n");
  printf("Option: ");
}

void handle_error(Status status) {
  switch (status) {
  case ERR_INVALID_INPUT:
    printf("Error: Invalid input. Please enter a number.\n\n");
    break;
  case ERR_INVALID_OPTION:
    printf("Error: Invalid option selected.\n\n");
    break;
  case ERR_MEMORY_ALLOCATION:
    printf("Error: Memory allocation failed.\n\n");
    break;
  case ERR_TREE_EMPTY:
    printf("Error: Tree is empty.\n\n");
    break;
  case ERR_DUPLICATE_VAL:
    printf("Error: Duplicate value not allowed in BST.\n\n");
    break;
  case ERR_NOT_FOUND:
    printf("Error: Value not found.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_insert(TreeNode **root) {
  int value;
  printf("\nValue to insert: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  Status status = insert_node(root, value);
  if (status == SUCCESS) {
    printf("\n  - Node [%d] inserted.\n\n", value);
  } else {
    handle_error(status);
  }
}

void run_search(TreeNode *root) {
  int value;
  printf("\nValue to search: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  TreeNode *found = search_node(root, value);
  if (found) {
    printf("\n  - Found: %d (Address: %p)\n\n", found->value, (void *)found);
  } else {
    handle_error(ERR_NOT_FOUND);
  }
}

void run_delete(TreeNode **root) {
  int value;
  printf("\nValue to delete: ");
  if (read_integer(&value) != SUCCESS) {
    handle_error(ERR_INVALID_INPUT);
    return;
  }

  printf("\nDeletion Log:\n");
  Status status = delete_node(root, value);

  if (status == SUCCESS) {
    printf("  - Node deleted successfully.\n\n");
  } else {
    handle_error(status);
  }
}

void run_traversals(TreeNode *root, int type) {
  if (root == NULL) {
    printf("\n  - Tree is empty.\n\n");
    return;
  }

  printf("\n");
  switch (type) {
  case 1:
    printf("Inorder (Sorted): ");
    print_inorder(root);
    break;
  case 2:
    printf("Preorder (Root First): ");
    print_preorder(root);
    break;
  case 3:
    printf("Postorder (Leaf First): ");
    print_postorder(root);
    break;
  case 4:
    printf("Level Order (Breadth First): ");
    print_level_order(root);
    break;
  }
  printf("\n\n");
}

void run_properties(TreeNode *root) {
  int h = get_height(root);
  int count = count_nodes(root);
  int balanced = is_balanced(root);

  printf("\nTree Statistics:\n");
  printf("  - Height (Max Depth): %d\n", h);
  printf("  - Total Nodes: %d\n", count);
  printf("  - Balanced: %s\n\n", balanced ? "YES" : "NO");
}

void run_count(TreeNode *root) {
  printf("\n  - Total nodes: %d\n\n", count_nodes(root));
}

void run_visualize(TreeNode *root) {
  if (root == NULL) {
    printf("\n  - Tree is empty.\n\n");
    return;
  }

  printf("\nVisual Tree (Rotated Left):\n");
  printf("(Right child is UP, Left child is DOWN)\n\n");
  print_tree_structure(root, 0);
  printf("\n");
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

Status insert_node(TreeNode **root, int value) {
  if (*root == NULL) {
    *root = (TreeNode *)malloc(sizeof(TreeNode));
    if (*root == NULL) {
      return ERR_MEMORY_ALLOCATION;
    }
    (*root)->value = value;
    (*root)->left = NULL;
    (*root)->right = NULL;
    return SUCCESS;
  }

  if (value < (*root)->value) {
    return insert_node(&((*root)->left), value);
  } else if (value > (*root)->value) {
    return insert_node(&((*root)->right), value);
  } else {
    return ERR_DUPLICATE_VAL;
  }
}

TreeNode *find_min(TreeNode *root) {
  while (root->left != NULL) {
    root = root->left;
  }

  return root;
}

Status delete_node(TreeNode **root, int value) {
  if (*root == NULL) {
    return ERR_NOT_FOUND;
  }

  if (value < (*root)->value) {
    return delete_node(&((*root)->left), value);
  } else if (value > (*root)->value) {
    return delete_node(&((*root)->right), value);
  } else {
    if ((*root)->left == NULL && (*root)->right == NULL) {
      printf("  - Leaf node found. Removing directly.\n");
      free(*root);
      *root = NULL;
    } else if ((*root)->left == NULL) {
      printf("  - Node with 1 child (Right) found. Re-linking.\n");
      TreeNode *temp = *root;
      *root = (*root)->right;
      free(temp);
    } else if ((*root)->right == NULL) {
      printf("  - Node with 1 child (Left) found. Re-linking.\n");
      TreeNode *temp = *root;
      *root = (*root)->left;
      free(temp);
    } else {
      printf("  - Node with 2 children found.\n");
      TreeNode *temp = find_min((*root)->right);
      printf("  - Replacing with Inorder Successor: %d\n", temp->value);
      (*root)->value = temp->value;
      delete_node(&((*root)->right), temp->value);
    }

    return SUCCESS;
  }
}

TreeNode *search_node(TreeNode *root, int value) {
  if (root == NULL || root->value == value) {
    return root;
  }

  if (value < root->value) {
    return search_node(root->left, value);
  }

  return search_node(root->right, value);
}

void print_inorder(TreeNode *root) {
  if (root != NULL) {
    print_inorder(root->left);
    printf("%d ", root->value);
    print_inorder(root->right);
  }
}

void print_preorder(TreeNode *root) {
  if (root != NULL) {
    printf("%d ", root->value);
    print_preorder(root->left);
    print_preorder(root->right);
  }
}

void print_postorder(TreeNode *root) {
  if (root != NULL) {
    print_postorder(root->left);
    print_postorder(root->right);
    printf("%d ", root->value);
  }
}

void print_level_order(TreeNode *root) {
  if (root == NULL) {
    return;
  }

  TreeNode *queue[MAX_QUEUE_SIZE];
  int front = 0, rear = 0;

  queue[rear++] = root;

  while (front < rear) {
    TreeNode *current = queue[front++];
    printf("%d ", current->value);

    if (current->left != NULL) {
      queue[rear++] = current->left;
    }
    if (current->right != NULL) {
      queue[rear++] = current->right;
    }
  }
}

int get_height(TreeNode *root) {
  if (root == NULL) {
    return -1;
  }

  int left_h = get_height(root->left);
  int right_h = get_height(root->right);
  return (left_h > right_h ? left_h : right_h) + 1;
}

int count_nodes(TreeNode *root) {
  if (root == NULL) {
    return 0;
  }

  return 1 + count_nodes(root->left) + count_nodes(root->right);
}

int is_balanced(TreeNode *root) {
  if (root == NULL) {
    return 1;
  }

  int lh = get_height(root->left);
  int rh = get_height(root->right);

  if (abs(lh - rh) <= 1 && is_balanced(root->left) &&
      is_balanced(root->right)) {
    return 1;
  }

  return 0;
}

void free_tree(TreeNode *root) {
  if (root != NULL) {
    free_tree(root->left);
    free_tree(root->right);
    free(root);
  }
}

void print_tree_structure(TreeNode *root, int space) {
  int COUNT = 5;
  if (root == NULL) {
    return;
  }

  space += COUNT;

  print_tree_structure(root->right, space);

  printf("\n");
  for (int i = COUNT; i < space; i++) {
    printf(" ");
  }
  printf("%d\n", root->value);

  print_tree_structure(root->left, space);
}
