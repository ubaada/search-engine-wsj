#include "linked_list.h"
#include <stdlib.h>
#include <stdio.h>

/* Create a linked list */
LinkedList* linkedlist_create(int (*cmp)(const void *, const void *)){
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
    if (list == NULL) {
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    list->cmp = cmp;
    return list;
}


/* Delete the linked list */
void linkedlist_delete(LinkedList *list) {
    Node *current = list->head;
    while (current != NULL) {
        Node *toDelete = current;
        current = current->next;
        free(toDelete->data); // Assuming data was dynamically allocated
        free(toDelete);
    }
    free(list);
}

/* Add node on top of the linked list */
bool linkedlist_add_head(LinkedList *list, void *data) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        return false;
    }
    newNode->data = data;
    newNode->next = list->head;
    list->head = newNode;

    if (list->tail == NULL) { // If list was empty, new node is also the tail
        list->tail = newNode;
    }

    return true;
}

/* Add node at the end of the linked list */
bool linkedlist_add_tail(LinkedList *list, void *data) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        return false;
    }
    newNode->data = data;
    newNode->next = NULL;

    if (list->tail == NULL) { // If list is empty, new node is both head and tail
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode; // Update last node's next pointer
        list->tail = newNode; // Update the tail pointer
    }

    return true;
}


/* Delete a node from the linked list */
bool linkedlist_delete_node(LinkedList *list, void *data) {
    Node **indirect = &list->head;

    while (*indirect != NULL) {
        if (list->cmp((*indirect)->data, data) == 0) {
            Node *toDelete = *indirect;
            *indirect = (*indirect)->next;
            free(toDelete->data); // Assuming data was dynamically allocated
            free(toDelete);
            return true;
        }
        indirect = &(*indirect)->next;
    }
    return false;
}

/* Search for a node in the linked list */
Node* linkedlist_search(LinkedList *list, void *data) {
    Node *current = list->head;
    while (current != NULL) {
        if (list->cmp(current->data, data) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/* Print the linked list */
void linkedlist_print(LinkedList *list, void (*my_print)(const void *)) {
    Node *current = list->head;
    while (current != NULL) {
        my_print(current->data);
        current = current->next;
    }
}


/* Function to split the list into two halves */
void split_list(Node* source, Node** frontRef, Node** backRef) {
    Node* fast;
    Node* slow;
    slow = source;
    fast = source->next;

    // Fast advances two nodes, and slow advances one node
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint in the list, so split it in two at that point
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}

/* Function to merge two sorted lists */
Node* merge_sorted_lists(Node* a, Node* b, int (*cmp)(const void *, const void *)) {
    Node* result = NULL;

    // Base cases
    if (a == NULL) return b;
    else if (b == NULL) return a;

    // Pick either a or b, and recur
    if (cmp(a->data, b->data) <= 0) {
        result = a;
        result->next = merge_sorted_lists(a->next, b, cmp);
    } else {
        result = b;
        result->next = merge_sorted_lists(a, b->next, cmp);
    }
    return result;
}

/* MergeSort function for linked list  */
void merge_sort(Node** headRef, int (*cmp)(const void *, const void *)) {
    Node* head = *headRef;
    Node* a;
    Node* b;

    // Base case -- length 0 or 1
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    // Split head into 'a' and 'b' sublists
    split_list(head, &a, &b);

    // Recursively sort the sublists
    merge_sort(&a, cmp);
    merge_sort(&b, cmp);

    // Merge the two sorted lists together
    *headRef = merge_sorted_lists(a, b, cmp);
}

/* Sort the linked list */
void linkedlist_sort(LinkedList *list) {
    if (list == NULL || list->head == NULL) {
        return; // The list is empty or has a single element
    }
    merge_sort(&(list->head), list->cmp);

    // Update the tail pointer after sorting
    Node *current = list->head;
    while (current != NULL && current->next != NULL) {
        current = current->next;
    }
    list->tail = current;
}
