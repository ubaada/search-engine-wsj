
## Design

### parser.c

This file reads and processes a WSJ XML dataset, outputting words from the file one per line to standard output. It produces a stream of words with extra newlines between documents, stemming words (except for document IDs) in the process.

### indexer.c

This file creates an index for a search engine by processing a stream of words and document IDs. It produces three files: a list of document IDs (`doc_id_list.txt`), a dictionary file with byte offsets to posting lists (`dict_and_offset.bin`), and a posting list file with document ID indexes and frequencies (`posting_list.bin`).

### searcher.c

This file takes a list of words as input and finds documents containing all the words by searching the previously created index. It produces a ranked and sorted list of document IDs that contain all the search words, along with their relevance scores.
          

## Usage
Compile All:
```
make All
```

Parser
```
./bin/parser <input_file> > <output_file>
```

Indexer
```
./bin/indexer <output_file>
```

Searcher
```
./bin/searcher word1 word2 word3 ... wordN
```
