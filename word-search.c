#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

void word_search(struct TrieNode *root, char *text, FILE *outFile){
	int counter;
	while(counter != '\0'){
		char *word = malloc(MAX_WORD_SIZE);
		memset((void *)word, 0, MAX_WORD_SIZE); 
		while(text[counter]!=' ' && text[counter]!='\n' && text[counter]!='\0'){
			strncat(word, &text[counter], 1);
		}
		if(search(root, word)){
			
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
    long int size;

    if (argc<2){
        fputs("Not enough arguments.\n",stderr);
        exit(EXIT_FAILURE);
    }
    inFile=fopen(argv[1],"rt");
    if(!inFile){
        fputs("Unable to open file.\n",stderr);
        exit(EXIT_FAILURE);
    }

	fseek(inFile, 0L, SEEK_END);
	size = ftell(inFile);
	fseek(inFile,0L,SEEK_SET);
	text = malloc(size);

	while(true){
		char temp=(char)fgetc(inFile);
		if(temp==EOF){
			break;
		}
		strncat(text,&temp,1);
	}
	fclose(inFile);
	outFile=fopen("out.txt","wt");
}
