/**
 * @file common.h
 * @brief Header file for linked list implementation.
 *
 * This file defines the structure and function prototypes for a generic linked list.
 * It supports operations such as addition and deletion of nodes, searching, sorting,
 * and printing of the linked list.
 *
 * @author Ubaada
 * @date 01-04-2024
 */

#ifndef COMMON_H
#define COMMON_H
#include <stddef.h>
#include <stdio.h>

/**
 * Define data sizes for the index
 */
#define MAX_KEY_SIZE 60
#define OFFSET_SIZE 4
#define DOC_ID_SIZE 14

/**
 * Define the structure of a posting 
 */
typedef struct Posting {
    int doc_id;
    int freq;
} Posting;

/**
 * Stem the given word by removing common suffixes 
 * Lowercases the word before stemming
 * Important that the same function is used for both indexing and searching
 * 
 * @param word The word to stem
*/
void stem(char *word);

/**
 * Check if two postings are same based on their doc_id
 * 
 * @param a The first posting
 * @param b The second posting
 * @return A negative number if a < b, 0 if a == b, a positive number if a > b
*/
int posting_cmp(const void *a, const void *b);

/**
 * Explicitly write an integer to a file in big-endian format.
 * Avoids conflicts with default endianness of the system.
 * 
 * @param file The file to write to
 * @param value The integer to write
 *
 */
void write_int_big_endian(FILE* file, int value);


/**
* Pack an integer using variable byte encoding and write it to a file
*
* @param n The integer to encode
* @param fp The file pointer to write to
*/
int variable_byte_encode(int n, FILE *fp);

/**
 * Read an integer from a file in big-endian format
 * Avoids differences in endianness between platforms
*/
int read_int_big_endian(FILE* file);
#endif

