/* Assignment 1: Indexer
 *
 * @file indexer.c
 * @brief Uses the parsed data to create an index for the search engine.
 *
 * This program creates (from a stream of words and document IDs)
 * 1. A list of document IDs
 * 2. A dictionary file with byte offsets to posting lists
 * 3. A posting list file with doc_id index and frequency
 *     i.  Delta encoding for doc_id
 *     ii. Further Variable byte encoding for doc_id and frequency
 * 
 * @author Ubaada
 * @date 01-04-2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "include/rbtree.h"
#include "include/linked_list.h"
#include "include/common.h"

#define ID_FILE "data/doc_id_list.txt" /* DOC ID list to convert index to doc_id */
#define DICT_FILE "data/dict_and_offset.bin" /* Dictionary file with byte offset to posting list */
#define POSTING_FILE "data/posting_list.bin" /* Posting list file, contains doc_id index and freq */


/**
 * Save the list of document IDs to a file
 * Produces: data/doc_id_list.txt
 * 
 * @param list The linked list of document IDs
 */
void save_id_list(LinkedList *list) {
    FILE *fp = fopen(ID_FILE, "wb");
    if (fp == NULL) {
        printf("Error: Couldn't open file for writing\n");
        return;
    }

    Node *current = list->head;
    while (current != NULL) {
        /* Newline separated list of document IDs */
        if (current->next != NULL) {
            fprintf(fp, "%s\n", (char *)current->data);
        } else {
            fprintf(fp, "%s", (char *)current->data);
        }
        current = current->next;
    }

    fclose(fp);
}


/* 
Recursive Helper function to write the posting lists & dictionary to files
*/
void _write_dict_postings(RBTree *tree, RBTreeNode *node, FILE *fp_post, FILE *fp_dict, int* byte_offset) {
    if (node == tree->nil) return;

    _write_dict_postings(tree, node->left, fp_post, fp_dict, byte_offset);

    /* Write the key to the dictionary file in MAX_KEY_SIZE bytes
     * followed by the byte offset in 4 bytes */
    fwrite(node->key, sizeof(char), MAX_KEY_SIZE, fp_dict);
    write_int_big_endian(fp_dict, *byte_offset);

    LinkedList *list = (LinkedList *)node->value;
    Node *current = list->head;

    /* Previous doc_id for delta encoding */
    int prev_doc_id = 0;
    while (current != NULL) {
        /* Write index and freq to the posting file.
        *  Total bytes written is tracked for offset of the next word
        */
        Posting *posting = (Posting *)current->data;
        int doc_id_delta = posting->doc_id - prev_doc_id;
        int id_bytes = variable_byte_encode(doc_id_delta, fp_post);
        int freq_bytes = variable_byte_encode(posting->freq, fp_post);
        current = current->next;
        *byte_offset += id_bytes + freq_bytes;
        prev_doc_id = posting->doc_id;

    }

    _write_dict_postings(tree, node->right, fp_post, fp_dict, byte_offset);
}

/** 
 * Write the dictionary and posting list to files
 * 
 * produces:    data/posting_list.bin
 *              data/dict_and_offset.bin
 * 
 * @param tree The tree to write to file
 * Each node in the tree is a word with a linked list of postings
*/
void write_dict_postings(RBTree *tree) {
    FILE* fp_post = fopen(POSTING_FILE, "wb");
    FILE* fp_dict = fopen(DICT_FILE, "wb");
    if (fp_post == NULL || fp_dict == NULL) {
        printf("Couldn't open file for index creation\n");
        return;
    }
    int byte_offset = 0;
    _write_dict_postings(tree, tree->root, fp_post, fp_dict, &byte_offset);
    fclose(fp_post);
    fclose(fp_dict);
}

/**
 * Main function to parse the given file.
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }
    
    printf("Opening file: '%s'\n", argv[1]);

    FILE* fp = fopen(argv[1], "rb"); /* File which contains the parsed data */
    if (fp == NULL) {
        printf("Error: Couldn't open file\n");
        return 1;
    }

    LinkedList* id_list = linkedlist_create(NULL); /* list of document IDs */
    RBTree* myTree = rb_create(); /* Dictionary tree with postings attached */
    int progress_counter = 0; /* Counter to track progress */
    char line[255]; /* Buffer to read lines from file */


    /* first line is ID */
    fgets(line, sizeof(line), fp);
    line[strcspn(line, "\n")] = 0;
    linkedlist_add_tail(id_list, strdup(line));

    int doc_index = 0; /*assigned to each document in the order they appear */
    while (fgets(line, sizeof(line), fp)) {
        if (strcmp(line, "\n") == 0) {
            /* Read the next line as an ID */
            fgets(line, sizeof(line), fp);
            line[strcspn(line, "\n")] = 0;
            linkedlist_add_tail(id_list, strdup(line));
            doc_index += 1;
            continue;
        }

        progress_counter += 1;

        /*
         * Check if the word is already in the tree 
         * If not, create a new linked list for the word
         * If it is, add a new posting to the linked list
         */
        line[strcspn(line, "\n")] = 0;
        //printf("Word: %s\n", line);
        RBTreeNode* btree_node = rb_search(myTree, line);
        if (btree_node == myTree->nil) {
            /* Word Not found, insert a new word with 1 new posting */
            LinkedList* new_list = linkedlist_create(posting_cmp);
            Posting* new_posting = (Posting *)malloc(sizeof(Posting));
            new_posting->doc_id = doc_index;
            new_posting->freq = 1;
            linkedlist_add_tail(new_list, new_posting);

            /* insert the word as key and the posting linked list as value */
            rb_insert(myTree, line, new_list);
        } else {
            /*
             * Btree node for the word Found, 
             * Add a new posting for this docid
             * if the docid is already present, increment the freq
             */
            LinkedList* posting_list = (LinkedList *)btree_node->value;
            Posting* last_posting = (Posting *)posting_list->tail->data;
            if (last_posting->doc_id == doc_index) {
                last_posting->freq += 1;
            } else {
                Posting* new_posting = (Posting *)malloc(sizeof(Posting));
                new_posting->doc_id = doc_index;
                new_posting->freq = 1;
                linkedlist_add_tail(posting_list, new_posting);
            }
        }

        /* Print progress */
        if (progress_counter % 1000000 == 0) {
            printf("\rWords: %d\n", progress_counter);
            fflush(stdout);
        }
    }

    /* Save the list of document IDs to a file */
    save_id_list(id_list);

    /* write the dictionary and posting list to files */
    write_dict_postings(myTree);

    /* Clean up */
    rb_destroy(myTree);
    linkedlist_delete(id_list);
    fclose(fp);

    return 0;
}

