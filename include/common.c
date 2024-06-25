/**
 * @file common.c
 * @brief Common functions for the search engine
 */

#include "common.h"
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>


/* Helper function to check if a string ends with a given suffix. */
bool ends_with(const char *str, const char *suffix) {
    if (!str || !suffix) return false;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr) return false;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}


/* Stem a char in place by removing suffixes if greater than a certain length */
void stem(char *word) {

    /* lowercase the word */
    for (int i = 0; word[i]; i++) {
        word[i] = tolower(word[i]);
    }

    /* sorted from longest to shortest */
    const char *suffixes[] = { "able", "ible", "ness", "ment", "ions", "ings", "ies", "ion", "ing", \
            "ate", "ize", "ise", "ant", "ent", "ful", "ous", "ive", \
            "es", "er", "or", "al", "ic", "ly", "ed", \
            "en", "fy", "\'s", "s", NULL};

    const char **suffix = suffixes;

    
    int too_short = 3; /* Minimum length of word after stemming */

    
    while (*suffix != NULL) {

        /* if word ends with suffix and removing it doesn't make the word too short */
        int result_len = strlen(word) - strlen(*suffix);
        if (ends_with(word, *suffix) && (result_len >= too_short)) {
            word[strlen(word) - strlen(*suffix)] = '\0'; /* Remove the suffix. */
            break;
        }

        suffix++;
    }
    return;
}

/* Compare two postings based on their doc_id */
int posting_cmp(const void *a, const void *b) {
    return ((Posting *)a)->doc_id - ((Posting *)b)->doc_id;
}

/*
* Explicitly write an integer to a file in big-endian format.
* Avoids conflicts with default endianness of the system.
*/
void write_int_big_endian(FILE* file, int value) {
    unsigned char bytes[4];

    bytes[0] = (value >> 24) & 0xFF; /* Most significant byte */
    bytes[1] = (value >> 16) & 0xFF;
    bytes[2] = (value >> 8) & 0xFF;
    bytes[3] = value & 0xFF; /* Least significant byte */

    fwrite(bytes, sizeof(bytes), 1, file);
}


/*
* Pack an integer using variable byte encoding and write it to a file
*/
int variable_byte_encode(int n, FILE *fp) {
    unsigned char buffer[10]; /* Buffer to hold encoded bytes */
    int i = 0;

    while (n > 0) {
        buffer[i] = n & 127; /* Get the lower 7 bits */
        n = n >> 7;
        i += 1;
    }
    if (i == 0) { /* if given number is 0 */
        buffer[0] = 0;
        i += 1;
    }
    buffer[0] = buffer[0] | 128; /* Set the first bit to 1 to indicate more bytes are coming */

    /* Process in reverse */
    for (int j = i - 1; j >= 0; j--) {
        fwrite(&buffer[j], sizeof(char), 1, fp);
    }
    return i;
}

/* Read an integer from a file in big-endian format */
int read_int_big_endian(FILE* file) {
    unsigned char bytes[OFFSET_SIZE];
    fread(bytes, sizeof(bytes), 1, file);

    return ((int)bytes[0] << 24) |
           ((int)bytes[1] << 16) |
           ((int)bytes[2] << 8) |
            (int)bytes[3];
}
