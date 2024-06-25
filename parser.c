/* Assignment 1: Parser
 *
 * @file parser.c
 * @brief A simple parser for reading and processing WSJ XML dataset.
 *
 * This program reads a file and outputs words from the file, one per line.
 * on the standard output.
 * Extra newlines are added between documents.
 * 
 * @author Ubaada
 * @date 01-04-2024
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "include/common.h"

#define BUFFER_SIZE 4096
char READ_BUFFER[BUFFER_SIZE];


/**
 * Parse the given document and output words to stdout.
 * 
 * Working: 
 * 1. Scan byte by byte to form a word until a non-alphanumeric char is found.
 * 2. Unless the char is between < and >, then it is a tag.
 * 3. If the tag is DOC, then the next word is a document ID.
 * 
 * 
 * @param doc_address The address of the document to parse.
*/
int parse(char *doc_address) {
    FILE *file = fopen(doc_address, "rb");
    if (file == NULL) {
        printf("Error: File not found\n");
        return 1;
    }

    char word[255];
    int word_index = 0;
    bool is_angle_start = false;
    bool is_angle_end = false;
    bool is_doc_id = false;
    bool is_first_doc = true;
    while (!feof(file)) {
        int bytes_read = fread(READ_BUFFER, 1, BUFFER_SIZE, file);
        for (int i = 0; i < bytes_read; i++) {
            if (READ_BUFFER[i] == '<') {
                is_angle_start = true;
            } else if (READ_BUFFER[i] == '>' && is_angle_start) {
                is_angle_end = true;
            }
            /**
             * STOP string building If current char is not alphanumeric
             * But allow '-' if it is a doc id
             */
            if ((isalnum(READ_BUFFER[i]) == 0) && !(is_doc_id && READ_BUFFER[i] == '-')) {
                if (word_index == 0) {
                    /* word is empty, skip */
                    continue;
                }
                if (is_angle_start && is_angle_end) {
                    /* tag between < and > */
                    is_angle_start = false;
                    is_angle_end = false;
                    word[word_index] = '\0';
                    if (strncmp(word, "DOC", word_index) == 0) {
                        /* so it only triggers for opening tag
                         * not both opening and closing tag
                         */
                        if (READ_BUFFER[i-4] == '<') {
                            if (!is_first_doc) {
                                printf("\n");
                            } else {
                                is_first_doc = false;
                            }
                            is_doc_id = true;
                        }
                    }
                    word_index = 0;
                } else {
                    /* word */
                    word[word_index] = '\0';
                    /* if it is a doc id, don't stem */
                    if (is_doc_id) {
                        is_doc_id = false;
                    } else {
                        stem(word);
                    }
                    printf("%s\n", word);
                    word_index = 0;
                }
            } else {
                /* add char to word */
                word[word_index++] = READ_BUFFER[i];
            }
        }
    }
    fclose(file);

    return 0;
}

/**
 * Main function to parse the given file.
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    /* read first argument as file name */
    int p = parse(argv[1]);

    return p;
}