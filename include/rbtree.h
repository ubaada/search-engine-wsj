#ifndef RBTREE_H
#define RBTREE_H

#include "common.h"

typedef enum { RED, BLACK } RBColor;

/* Define the node structure */
typedef struct RBTreeNode {
    // key is string
    char key[MAX_KEY_SIZE];
    void *value;
    RBColor color;
    struct RBTreeNode *left;
    struct RBTreeNode *right;
    struct RBTreeNode *parent;
} RBTreeNode;

/* Define the tree structure */
typedef struct RBTree {
    RBTreeNode *root;
    RBTreeNode *nil; // Sentinel node to represent leaves
} RBTree;

/* Create a new tree */
RBTree* rb_create();

/* Node insertion */
void rb_insert(RBTree *tree, char* key, void *value); //<-- Change to char *key

/* Node deletion */
void rb_delete(RBTree *tree, RBTreeNode *node); 

/* Search for a node */
RBTreeNode* rb_search(RBTree *tree, char* key); //<-- Change to char *key

/* Minimum and maximum */
RBTreeNode* rb_minimum(RBTree *tree, RBTreeNode *node);
RBTreeNode* rb_maximum(RBTree *tree, RBTreeNode *node);

/* Successor and predecessor */
RBTreeNode* rb_successor(RBTree *tree, RBTreeNode *node);
RBTreeNode* rb_predecessor(RBTree *tree, RBTreeNode *node);

/* Insert or check if key is already in the tree
  Returns 1 if key was NOT in the tree and was inserted, 0 otherwise */
int rb_insert_or_check(RBTree *tree, char* key, void *value); //<-- Change to char *key

/* Inorder print all nodes if n = -1, else print n nodes */
void rb_inorder_traversal(RBTree *tree, void (*func)(RBTreeNode *), int n);

/* Destroy the tree */
void rb_destroy(RBTree *tree);

#endif // RBTREE_H
