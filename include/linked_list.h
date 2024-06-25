/**
 * @file linked_list.h
 * @brief Header file for linked list implementation.
 *
 * This file defines the structure and function prototypes for a generic linked list.
 * It supports operations such as addition and deletion of nodes, searching, sorting,
 * and printing of the linked list.
 *
 * @author Ubaada
 * @date 01-04-2024
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdbool.h>

/* Define a node of the linked list */
typedef struct Node {
    void *data;  
    struct Node *next;  
} Node;

/* linked list structure */
typedef struct LinkedList {
    Node *head;                      
    Node *tail;  
    int (*cmp)(const void *, const void *); /* Function pointer for comparing two nodes */
} LinkedList;


/** 
 * Create and initialize a new linked list.
 * 
 * @param cmp A function pointer to compare two elements. It should return a negative number,
 * zero, or a positive number depending on whether the first argument is less than, equal to,
 * or greater than the second.
 * @return A pointer to the newly created linked list.
 */
LinkedList* linkedlist_create(int (*cmp)(const void *, const void *));

/** 
 * Delete the linked list and free all associated memory.
 * 
 * @param list A pointer to the linked list to be deleted.
 */
void linkedlist_delete(LinkedList *list);

/** 
 * Add a new node at the head of the linked list.
 * 
 * @param list A pointer to the linked list.
 * @param data The data to be added.
 * @return True if the operation was successful, otherwise false.
 */
bool linkedlist_add_head(LinkedList *list, void *data);

/** 
 * Add a new node at the tail of the linked list.
 * 
 * @param list A pointer to the linked list.
 * @param data The data to be added.
 * @return True if the operation was successful, otherwise false.
 */
bool linkedlist_add_tail(LinkedList *list, void *data);

/** 
 * Delete a node from the linked list.
 * 
 * @param list A pointer to the linked list.
 * @param data The data to be deleted.
 * @return True if the operation was successful, otherwise false.
 */
bool linkedlist_delete_node(LinkedList *list, void *data);

/** 
 * Search for a node in the linked list.
 * 
 * @param list A pointer to the linked list.
 * @param data The data to search for.
 * @return A pointer to the node if found, otherwise NULL.
 */
Node* linkedlist_search(LinkedList *list, void *data);

/** 
 * Print the linked list.
 * 
 * @param list A pointer to the linked list.
 * @param my_print A function pointer to print the data of a node.
 */
void linkedlist_print(LinkedList *list, void (*my_print)(const void *));


/** 
 * Sort the linked list using the comparison function defined in the list.
 * 
 * @param list A pointer to the linked list to be sorted.
 */
void linkedlist_sort(LinkedList *list);


#endif // LINKED_LIST_H
