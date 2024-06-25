

all: searcher indexer parser
FLAGS = -Wall -Wextra -Werror -pedantic

clean:
	rm -rf ./bin/*
	
searcher: searcher.c
	gcc -o ./bin/searcher searcher.c ./include/* $(FLAGS)

indexer: indexer.c
	gcc -o ./bin/indexer indexer.c ./include/* $(FLAGS)

parser: parser.c
	gcc -o ./bin/parser parser.c ./include/* $(FLAGS)