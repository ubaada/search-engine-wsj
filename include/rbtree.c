#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* Initialize the red-black tree */
void rb_initialize(RBTree *tree) {
    tree->nil = (RBTreeNode *)malloc(sizeof(RBTreeNode));
    tree->nil->color = BLACK;
    
    tree->nil->left = tree->nil->right = tree->nil->parent = tree->nil;
    tree->root = tree->nil;
}

/* Create a new tree */
RBTree* rb_create() {
    RBTree *tree = (RBTree *)malloc(sizeof(RBTree));
    if (tree == NULL) {
        return NULL;
    }
    rb_initialize(tree);
    return tree;
}



/* Rotate left */
static void left_rotate(RBTree *tree, RBTreeNode *x) {
    RBTreeNode *y = x->right;
    x->right = y->left;
    if (y->left != tree->nil) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == tree->nil) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

/* Rotate right */
static void right_rotate(RBTree *tree, RBTreeNode *x) {
    RBTreeNode *y = x->left;
    x->left = y->right;
    if (y->right != tree->nil) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == tree->nil) {
        tree->root = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

/* Fix the red-black tree properties after insertion */
static void rb_insert_fixup(RBTree *tree, RBTreeNode *z) {
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            RBTreeNode *y = z->parent->parent->right;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(tree, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate(tree, z->parent->parent);
            }
        } else {
            RBTreeNode *y = z->parent->parent->left;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(tree, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate(tree, z->parent->parent);
            }
        }
    }
    tree->root->color = BLACK;
}

/* Insert a new node into the tree */
void rb_insert(RBTree *tree, char* key, void *value) {
    RBTreeNode *newNode = (RBTreeNode *)malloc(sizeof(RBTreeNode));
    // Zero out the key array
    memset(newNode->key, 0, MAX_KEY_SIZE);
    strcpy(newNode->key, key);
    newNode->value = value;
    newNode->left = newNode->right = newNode->parent = tree->nil;
    newNode->color = RED; // New nodes are always inserted red

    RBTreeNode *y = tree->nil; // Trailing pointer for the parent of the insert node
    RBTreeNode *x = tree->root; // Current node, starting at the root

    // Find the correct position for the new node in the tree
    while (x != tree->nil) {
        y = x;
        if (strcmp(newNode->key, x->key) < 0) { // <-- comparison
            x = x->left;
        } else {
            x = x->right;
        }
    }

    // y is parent of x
    newNode->parent = y;
    if (y == tree->nil) {
        // Tree was empty
        tree->root = newNode;
    } else if (strcmp(newNode->key, y->key) < 0) {  // <-- comparison
        y->left = newNode;
    } else {
        y->right = newNode;
    }

    // Fix up the red-black tree properties
    rb_insert_fixup(tree, newNode);
}

/* Transplant the subtree rooted at node u with the subtree rooted at node v */
static void rb_transplant(RBTree *tree, RBTreeNode *u, RBTreeNode *v) {
    if (u->parent == tree->nil) {
        tree->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

/* Fix the red-black tree properties after deletion */
static void rb_delete_fixup(RBTree *tree, RBTreeNode *x) {
    while (x != tree->root && x->color == BLACK) {
        if (x == x->parent->left) {
            RBTreeNode *w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                left_rotate(tree, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    right_rotate(tree, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                left_rotate(tree, x->parent);
                x = tree->root; // This will terminate the loop
            }
        } else { // Symmetric to the above case
            RBTreeNode *w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                right_rotate(tree, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    left_rotate(tree, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                right_rotate(tree, x->parent);
                x = tree->root; // This will terminate the loop
            }
        }
    }
    x->color = BLACK;
}

/* Delete a node from the tree */
void rb_delete(RBTree *tree, RBTreeNode *z) {
    RBTreeNode *y = z;
    RBTreeNode *x;
    RBColor originalColor = y->color;

    if (z->left == tree->nil) {
        x = z->right;
        rb_transplant(tree, z, z->right);
    } else if (z->right == tree->nil) {
        x = z->left;
        rb_transplant(tree, z, z->left);
    } else {
        y = rb_minimum(tree, z->right);
        originalColor = y->color;
        x = y->right;

        if (y->parent == z) {
            x->parent = y;
        } else {
            rb_transplant(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        rb_transplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (originalColor == BLACK) {
        rb_delete_fixup(tree, x);
    }

    free(z); // Finally, free the memory of the node being removed
}

/* Search for a node with the given key */
RBTreeNode* rb_search(RBTree *tree, char* key) {
    RBTreeNode *current = tree->root;
    while (current != tree->nil && strcmp(key, current->key) != 0) { // <-- comparison
        if (strcmp(key, current->key) < 0) { // <-- comparison
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return current; // Returns the node with the matching key or tree->nil if not found
}

/* Find the minimum and maximum nodes in the tree */
RBTreeNode* rb_minimum(RBTree *tree, RBTreeNode *node) {
    while (node->left != tree->nil) {
        node = node->left;
    }
    return node;
}

/* Find the maximum node in the tree */
RBTreeNode* rb_maximum(RBTree *tree, RBTreeNode *node) {
    while (node->right != tree->nil) {
        node = node->right;
    }
    return node;
}

/* Find the successor and predecessor of a node */
RBTreeNode* rb_successor(RBTree *tree, RBTreeNode *node) {
    RBTreeNode* minNode = NULL;
     if (node->right != tree->nil) {
         minNode = rb_minimum(tree, node->right);
     }
     return minNode;

    RBTreeNode *parent = node->parent;
    while (parent != tree->nil && node == parent->right) {
        node = parent;
        parent = parent->parent;
    }
    return parent;
}

/* Find the predecessor of a node */
RBTreeNode* rb_predecessor(RBTree *tree, RBTreeNode *node) {
    if (node->left != tree->nil) {
        return rb_maximum(tree, node->left);
    }

    RBTreeNode *parent = node->parent;
    while (parent != tree->nil && node == parent->left) {
        node = parent;
        parent = parent->parent;
    }
    return parent;
}

/* Insert a new node into the tree or check if the key already exists */
int rb_insert_or_check(RBTree *tree, char* key, void *value) {
    RBTreeNode *y = tree->nil;
    RBTreeNode *x = tree->root;
    RBTreeNode *z;

    // Find the correct insertion point (or existing key)
    while (x != tree->nil) {
        y = x;
        int cmp = strcmp(key, x->key);
        if (cmp == 0) {
            // Key already exists
            return 0; // Indicates that the key was found and not inserted
        }
        x = cmp < 0 ? x->left : x->right;
    }

    // Key not found, proceed with insertion
    z = (RBTreeNode *)malloc(sizeof(RBTreeNode));
    strncpy(z->key, key, MAX_KEY_SIZE - 1); // Copy with bounds checking
    z->key[MAX_KEY_SIZE - 1] = '\0'; // Ensure null-termination
    z->value = value;
    z->left = z->right = tree->nil;
    z->parent = y;
    z->color = RED; // New nodes are always inserted red in a red-black tree

    if (y == tree->nil) {
        tree->root = z; // The tree was empty, new node is now root
    } else if (strcmp(key, y->key) < 0) {
        y->left = z;
    } else {
        y->right = z;
    }

    RBTreeNode *node = z;
    rb_insert_fixup(tree, node); // Fix the red-black tree properties
    return 1; // Indicates that the key was successfully inserted
}

/* Inorder traversal of the tree */
void inorder_traversal_util(RBTree *tree, RBTreeNode *node, void (*func)(RBTreeNode *), int *count, int n) {
    if (node == tree->nil || *count == n) return;

    inorder_traversal_util(tree, node->left, func, count, n);
    
    if (*count < n || n == -1) {
        func(node); // Execute the given function on the node's key
        (*count)++;
    }

    inorder_traversal_util(tree, node->right, func, count, n);
}

/* Inorder traversal of the tree */
void rb_inorder_traversal(RBTree *tree, void (*func)(RBTreeNode *), int n) {
    int count = 0;
    inorder_traversal_util(tree, tree->root, func, &count, n);
}



static void rb_destroy_helper(RBTree *tree, RBTreeNode *node) {
    if (node != tree->nil) {
        rb_destroy_helper(tree, node->left);
        rb_destroy_helper(tree, node->right);
        free(node);
    }
}


void rb_destroy(RBTree *tree) {
    rb_destroy_helper(tree, tree->root);
    free(tree->nil); // Also free the sentinel node
    tree->root = tree->nil = NULL; // Set root and nil to NULL to mark the tree as destroyed
}
