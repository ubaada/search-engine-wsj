/* Assignment 1: Searcher
 *
 * @file searcher.c
 * @brief Takes a list of words and finds the documents that contain all the words.
 *
 * This program reads searches for the word in the dictionary.
 * It takes the offset from dictionary to locate and decompress the posting list.
 * Intersects the posting lists of all words to find the common documents.
 * Ranks the documents based on the frequency of the words and outputs the ordered list.
 * 
 * 
 * @author Ubaada
 * @date 01-04-2024
 */
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "include/linked_list.h"
#include "include/common.h"


#define ID_FILE "data/doc_id_list.txt"
#define DICT_FILE "data/dict_and_offset.bin"
#define POSTING_FILE "data/posting_list.bin"


/*
 * To store the search results
 */
typedef struct SearchResult {
    char doc_id[DOC_ID_SIZE + 1];
    float score;
} SearchResult;



/**
 * Decode a string of bytes encoded using variable byte encoding
 * into a list of integers and return it as a linked list of postings
 * id and frequency are read alternatively.
 * Also, undoes the delta and variable byte encoding.
 * 
 * @param data The encoded data
 * @param size The size of the data
 * @return A linked list of postings
 * 
 */
LinkedList* decode_posting_list(unsigned char *data, int size) {
    LinkedList* list = linkedlist_create(NULL);
    list->head = NULL;
    list->tail = NULL;

    int res = 0;
    int is_id = 1; /* First number is ID then freq alternatively */
    int prev_id = 0; /* For delta encoding */
    Posting *posting = NULL;
    for (int i = 0; i < size; i++) {
        if (!(data[i] & 128)) {
            /* keep adding offset byte and current byte
             * leading bit is 0 it will remove itself when added
             */
            res = (res | (int)data[i]) << 7;
        } else {
            /* Final byte of a number
             * Unset leading 1 and add
             */
            res = (res | (data[i] ^ 128));
            if (is_id) {
                posting = (Posting *)malloc(sizeof(Posting));
                prev_id += res;
                posting->doc_id = prev_id;
                is_id = 0;
            } else {
                posting->freq = res;
                linkedlist_add_tail(list, posting);
                is_id = 1;
            }
            res = 0;
        }
    }

    return list;
}


/**
 * Calculate the rank of the search results based on the frequency of the words
 * 
 * @param ranked_results The list of ranked results to be populated
 * @param results The list of search results
 * @param id_file The file containing the document IDs
 * 
*/
void calculate_rank(LinkedList* ranked_results, LinkedList* results, FILE* id_file) {
    Node *current = results->head;
    while (current != NULL) {
        Posting *posting = (Posting *)current->data;

        /* Get the DOC_ID from the index number */
        SearchResult *result = (SearchResult *)malloc(sizeof(SearchResult));
        fseek(id_file, posting->doc_id * (DOC_ID_SIZE + 1), SEEK_SET); 
        fread(result->doc_id, DOC_ID_SIZE, 1, id_file);
        result->doc_id[DOC_ID_SIZE] = '\0';
        /* Simple ranking based on the frequency of the word */
        result->score = posting->freq;
        
        /* Insert the result in the ranked list */
        linkedlist_add_tail(ranked_results, result);

        current = current->next;
    }
}

/**
 * Copy a posting list
 * 
 * @param original The original posting list
 * @return A copy of the posting list
 */
LinkedList* copy_posting_list(LinkedList *original) {
    LinkedList *copy = linkedlist_create(NULL); /* Assume this initializes an empty list */
    Node *current = original->head;
    while (current != NULL) {
        Posting *originalPosting = (Posting *)current->data;
        /* Allocate memory for a new Posting and copy the values */
        Posting *newPosting = malloc(sizeof(Posting));
        if (newPosting == NULL) {
            exit(1);
        }
        *newPosting = *originalPosting; /* Shallow copy is enough since fields are not pointers */

        linkedlist_add_tail(copy, newPosting); /* Add the new posting to the copied list */
        current = current->next;
    }
    return copy;
}

/**
 * Intersect the posting lists of all words to find the common documents
 * 
 * @param all_word_plist The list of all words' posting lists
 * @param resultList The resulting posting list
 */
void intersect_posting_lists(LinkedList *all_word_plist, LinkedList **resultList) {
    if (all_word_plist->head == NULL) return; /* Early return if no posting lists */

    /* Initialize the intersection result with the first posting list */
    LinkedList *results = copy_posting_list((LinkedList *)all_word_plist->head->data);

    Node *i = all_word_plist->head->next; /* Start from the second posting list */
    while (i != NULL) {
        LinkedList *currentList = (LinkedList *)i->data;
        LinkedList *newResults = linkedlist_create(NULL);

        /* Iterate over the results to see if each doc_id is in the current list */
        Node *resultNode = results->head;
        while (resultNode != NULL) {
            Posting *resultPosting = (Posting *)resultNode->data;
            int isInCurrentList = 0; /* Flag to check existence in the current list */

            Node *currentNode = currentList->head;
            while (currentNode != NULL) {
                Posting *currentPosting = (Posting *)currentNode->data;
                if (currentPosting->doc_id == resultPosting->doc_id) {
                    isInCurrentList = 1;
                    resultPosting->freq += currentPosting->freq; /* Sum frequencies */
                    break;
                }
                currentNode = currentNode->next;
            }

            if (isInCurrentList) {
                /* If the doc_id is in both lists, add it to the new results */
                Posting *newPosting = malloc(sizeof(Posting));
                newPosting->doc_id = resultPosting->doc_id;
                newPosting->freq = resultPosting->freq;
                linkedlist_add_tail(newResults, newPosting);
            }

            resultNode = resultNode->next;
        }

        /* Replace the old results with the newResults */
        linkedlist_delete(results);
        results = newResults;

        i = i->next;
    }

    /* Set the resultList to the final intersection of all posting lists */
    *resultList = results;
}

/**
 * Compare function for sorting search results
 * 
 * @param a The first search result
 * @param b The second search result
 * @return 1 if a > b, -1 if a < b, 0 if a == b
 
*/
int cmp_search_results(const void *a, const void *b) {
    SearchResult *resultA = (SearchResult *)a;
    SearchResult *resultB = (SearchResult *)b;

    if (resultA->score < resultB->score) {
        return 1;
    } else if (resultA->score > resultB->score) {
        return -1;
    } else {
        return 0;
    }
}

/**
 * Get the posting list for a word from the dictionary and posting list files
 * 
 * @param search_word The word to search for
 * @param dict_file The dictionary file
 * @param posting_file The posting list file
 * @param dict_size The size of the dictionary
 * @param all_word_plist The list of all words' posting lists
 * @return true if the word was found, false otherwise
 */
bool get_posting_list(char* search_word, FILE* dict_file, FILE* posting_file, int dict_size, LinkedList* all_word_plist) {
    char word[MAX_KEY_SIZE] = {0};
    stem(search_word); 
    int low = 0, high = dict_size - 1;
    int posting_begin_offset = -1;
    int posting_end_offset = -1;

    /* Binary search the word in the dictionary */
    while (low <= high) {
        int mid = low + (high - low) / 2;
        fseek(dict_file, mid * (MAX_KEY_SIZE+OFFSET_SIZE), SEEK_SET);

        fread(word, MAX_KEY_SIZE, 1, dict_file);
        
        int cmp = strcmp(search_word, word);
        if (cmp == 0) {
            /*
             * Found the word
             * Get the offset into the posting list file for this word
             */
            posting_begin_offset = read_int_big_endian(dict_file);
            /*
             * get the end offset from the start of next word 
             * or from the end of the file if this is the last word
             */
            if (mid < dict_size - 1) {
                fseek(dict_file, (mid + 1) * (MAX_KEY_SIZE + OFFSET_SIZE) + MAX_KEY_SIZE, SEEK_SET);
                posting_end_offset = read_int_big_endian(dict_file);
            } else {
                fseek(posting_file, 0, SEEK_END);
                posting_end_offset = ftell(posting_file);
            }
            break;
        } else if (cmp < 0) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }

    if (posting_begin_offset == -1) {
        /* Word not found */
        return false;
    }

    /* Read the posting list from the posting file */
    fseek(posting_file, posting_begin_offset, SEEK_SET);
    unsigned int p_size = posting_end_offset - posting_begin_offset; /* [begin, end) */
    unsigned char *data = (unsigned char *)malloc(p_size);
    fread(data, p_size, 1, posting_file);

    /* Decode the posting list into a linked list */
    LinkedList* word_plist = decode_posting_list(data, p_size);

    /* Append the list to the list of lists */
    linkedlist_add_tail(all_word_plist, word_plist);
    return true;
}

/**
 * Main function.
 * Takes a list of words and finds the documents that contain all the words
 */
int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: %s <word>\n", argv[0]);
        return 1;
    }

    /* Open the data files */
    FILE *dict_file = fopen(DICT_FILE, "rb");
    FILE *posting_file = fopen(POSTING_FILE, "rb");
    FILE *id_file = fopen(ID_FILE, "rb");
    struct stat sb;
    int stat_res = (fstat(fileno(dict_file), &sb) == -1);
    if (dict_file == NULL || posting_file == NULL || id_file == NULL || stat_res) {
        printf("Error: Error opening file(s)\n");
        return 1;
    }

    /* List of all words' posting lists */
    LinkedList *all_word_plist = linkedlist_create(NULL); 

    
    /* Search for each word in the dictionary */
    int dict_size = sb.st_size / (MAX_KEY_SIZE + OFFSET_SIZE);
    for (int i = 1; i < argc; i++) {
        bool found = get_posting_list(argv[i], dict_file, posting_file, dict_size, all_word_plist);
        if (!found) {
            /* If any one of the words is not found, exit */
            return 0;
        }
    }

    /* If no words were found, exit */
    if (all_word_plist->head == NULL) {
        return 0;
    }
    
    /* Intersect the posting lists of all words (AND search) */
    LinkedList *results = NULL;
    intersect_posting_lists(all_word_plist, &results);

    /* Rank the results and get DOC_ID from the ID file */
    LinkedList *ranked_results = linkedlist_create(cmp_search_results);
    calculate_rank(ranked_results, results, id_file);

    /* Sort the ranked results */
    linkedlist_sort(ranked_results);

    /* Print the ranked and sorted results */
    Node *current = ranked_results->head;
    while (current != NULL) {
        SearchResult *result = (SearchResult *)current->data;
        printf("%s %f\n", result->doc_id, result->score);
        current = current->next;
    }

    /* Clean up */
    fclose(dict_file);
    fclose(posting_file);
    fclose(id_file);
    linkedlist_delete(all_word_plist);
    linkedlist_delete(results);
    linkedlist_delete(ranked_results);

    return 0;
}