#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

//maximum word size
#define MAX_WORD_SIZE 16

#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

// Alphabet size (# of symbols)
#define ALPHABET_SIZE (26)

// Converts key current character into index
// use only 'a' through 'z' and lower case
#define CHAR_TO_INDEX(c) ((int)c - (int)'a')

// trie node
struct TrieNode
{
	struct TrieNode *children[ALPHABET_SIZE];

	// isEndOfWord is true if the node represents
	// end of a word
	bool isEndOfWord;
};

// Returns new trie node (initialized to NULLs)
struct TrieNode *getNode(void)
{
	struct TrieNode *pNode = NULL;

	pNode = (struct TrieNode *)malloc(sizeof(struct TrieNode));

	if (pNode)
	{
		int i;

		pNode->isEndOfWord = false;

		for (i = 0; i < ALPHABET_SIZE; i++)
			pNode->children[i] = NULL;
	}

	return pNode;
}

// If not present, inserts key into trie
// If the key is prefix of trie node, just marks leaf node
void insert(struct TrieNode *root, const char *key)
{
	int level;
	int length = strlen(key);
	int index;

	struct TrieNode *pCrawl = root;

	for (level = 0; level < length; level++)
	{
		index = CHAR_TO_INDEX(key[level]);
		if (!pCrawl->children[index])
			pCrawl->children[index] = getNode();

		pCrawl = pCrawl->children[index];
	}

	// mark last node as leaf
	pCrawl->isEndOfWord = true;
}

// Returns true if key presents in trie, else false
bool search(struct TrieNode *root, const char *key)
{
	int level;
	int length = strlen(key);
	int index;
	struct TrieNode *pCrawl = root;

	for (level = 0; level < length; level++)
	{
		index = CHAR_TO_INDEX(key[level]);

		if (!pCrawl->children[index])
			return false;

		pCrawl = pCrawl->children[index];
	}

	return (pCrawl != NULL && pCrawl->isEndOfWord);
}

struct thread_args{
	struct TrieNode *root;
	char *text;
	FILE *outFile;
};

void *routine1(void *args){
	struct thread_args arguments = *((struct thread_args *)args);
	char *text = arguments.text;
	FILE *outFile = arguments.outFile;
	struct TrieNode *root = arguments.root;
	int counter;
	int line = 1;
	clock_t t;
	while(counter != '\0'){
		char *word = malloc(MAX_WORD_SIZE);
		memset((void *)word, 0, MAX_WORD_SIZE); 
		while(text[counter]!=' ' && text[counter]!='\n' && text[counter]!='\0'){
			strncat(word, &text[counter], 1);
		}
		t = clock();
		if(search(root, word)){
			t = clock() - t;
			double time_elapsed = ((double)t)/CLOCKS_PER_SEC;
			fprintf(outFile, "Word: %s. Found in Line: %d. Time elapsed to be found: %f. Time elapsed to be written in the output file: %f\n", word, line, time_elapsed, time_elapsed);
		}
		if(text[counter]=='\n'){
			line++;
		}
		if(text[counter]!='\0'){
			counter++;
		}
	}
}

void *routine2(void *args){
	struct thread_args arguments = *((struct thread_args *)args);
	char *text = arguments.text;
	FILE *outFile = arguments.outFile;
	struct TrieNode *root = arguments.root;
	int counter;
	int line = 1;
	clock_t t;
	while(counter != '\0'){
		char *word = malloc(MAX_WORD_SIZE);
		memset((void *)word, 0, MAX_WORD_SIZE); 
		while(text[counter]!=' ' && text[counter]!='\n' && text[counter]!='\0'){
			strncat(word, &text[counter], 1);
		}
		t = clock();
		if(search(root, word)){
			t = clock() - t;
			double time_elapsed = ((double)t)/CLOCKS_PER_SEC;
			fprintf(outFile, "Word: %s. Found in Line: %d. Time elapsed to be found: %f. Time elapsed to be written in the output file: %f\n", word, line, time_elapsed, time_elapsed);
		}
		if(text[counter]=='\n'){
			line++;
		}
		if(text[counter]!='\0'){
			counter++;
		}
	}

}


void word_search(struct TrieNode *root, char *text, FILE *outFile){
	int counter;
	int line = 1;
	clock_t t;
	while(counter != '\0'){
		char *word = malloc(MAX_WORD_SIZE);
		memset((void *)word, 0, MAX_WORD_SIZE); 
		while(text[counter]!=' ' && text[counter]!='\n' && text[counter]!='\0'){
			strncat(word, &text[counter], 1);
		}
		t = clock();
		if(search(root, word)){
			t = clock() - t;
			double time_elapsed = ((double)t)/CLOCKS_PER_SEC;
			fprintf(outFile, "Word: %s. Found in Line: %d. Time elapsed to be found: %f. Time elapsed to be written in the output file: %f\n", word, line, time_elapsed, time_elapsed);
		}
		if(text[counter]=='\n'){
			line++;
		}
		if(text[counter]!='\0'){
			counter++;
		}
	}
}







int main(int argc, char* argv[])
{
	FILE* inFile;
	FILE* outFile;
    char* text;
    
    if (argc<2){
        fputs("Not enough arguments.",stderr);
        exit(EXIT_FAILURE);
    }
    inFile=fopen(argv[1],"rt");
    if(!inFile){
        fputs("Unable to open file.",stderr);
        exit(EXIT_FAILURE);
    }

	fseek(inFile, 0L, SEEK_END);
	long int size = ftell(inFile);
	

	text = malloc(sizeof(char)*size);
}
