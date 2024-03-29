#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#if __linux
#include <sys/syscall.h>
#endif
#include <ctype.h>
#include <semaphore.h>

//maximum word size
#define MAX_WORD_SIZE 46

//maximum line length
#define MAX_LINE_SIZE 200

//maximum number of words to search
#define MAX_WORD_NUM 20

//maximum number of threads
#define MAX_THREAD_NUM 4

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


struct thread_args{
	struct TrieNode *root;
	char *text;
	FILE *outFile;
	int line;
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
		// if(level>0){
		// 	pCrawl->children[index]=malloc(sizeof(struct TrieNode));
		// }
		if (!pCrawl->children[index])
			pCrawl->children[index] = getNode();

		pCrawl->children[index] = malloc(sizeof(struct TrieNode));
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

struct TrieNode create_trie_tree(struct TrieNode *root, char* list[],int size){
	for(int i=0;i<size;i++){
		insert(root,list[i]);
	}
}

sem_t mutex;
bool available = true;

//acquire and release for mutex locks
void acquire(){
	while(!available);
	available = false;
}

void release(){
	available = true;
}

void *routine1(void *args){
	struct thread_args arguments = *((struct thread_args *)args);
	char *text = arguments.text;
	FILE *outFile = arguments.outFile;
	struct TrieNode *root = arguments.root;
	int line = arguments.line;
	int counter;
	clock_t t, tf, to;
	t = clock();
#if __linux
	int tid = (int)syscall(SYS_gettid);
#else
	int tid = (int)pthread_self();
#endif

	while(text[counter] != '\0'){
		char *word = malloc(MAX_WORD_SIZE);
		memset((void *)word, 0, MAX_WORD_SIZE); 
		while(text[counter]!=' ' && text[counter]!='\n' && text[counter]!='\0' && text[counter]!=',' && text[counter]!='?' && text[counter]!='.' && text[counter]!='!' && text[counter]!=';' && text[counter]!=':' && text[counter]!=')'){
			strncat(word, &text[counter], 1);
			counter++;
		}
		bool rv=false;
		if(rv=search(root, word)){
			tf = clock() - t;
			sem_wait(&mutex);
			to = clock() - t;
			fprintf(outFile, "Word: %s. Found in Line: %d. Found by thread: %d. Time elapsed to be found: %ld. Time elapsed to be written in the output file: %ld\n", word, line, tid, tf, to);
			sem_post(&mutex);
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
	int line = arguments.line;
	int counter;
	clock_t t,tf,to;
	t = clock();
	while(text[counter] != '\0'){
		char *word = malloc(MAX_WORD_SIZE);
		memset((void *)word, 0, MAX_WORD_SIZE); 
		while(text[counter]!=' ' && text[counter]!='\n' && text[counter]!='\0' && text[counter]!=',' && text[counter]!='?' && text[counter]!='.' && text[counter]!='!' && text[counter]!=';' && text[counter]!=':' && text[counter]!=')'){
			strncat(word, &text[counter], 1);
			counter++;
		}
		if(search(root, word)){
			tf = clock() - t;
#if __linux
			int tid = (int)syscall(SYS_gettid);
#else
			int tid = (int)pthread_self();
#endif
			acquire();
			to = clock() - t;
			fprintf(outFile, "Word: %s. Found in Line: %d. Found by thread: %d. Time elapsed to be found: %ld. Time elapsed to be written in the output file: %ld\n", word, line, tid, tf, to);
			release();
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
	int counter=0;
	int line = 1;
	clock_t t;
	t = clock();
	while(text[counter] != '\0'){
		char *word = malloc(MAX_WORD_SIZE);
		memset((void *)word, 0, MAX_WORD_SIZE); 
		while(text[counter]!=' ' && text[counter]!='\n' && text[counter]!='\0' && text[counter]!=',' && text[counter]!='?' && text[counter]!='.' && text[counter]!='!' && text[counter]!=';' && text[counter]!=':' && text[counter]!=')'){
			strncat(word, &text[counter], 1);
			counter++;
		}
		strcat(word,"\0");
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

void thread_driver(int choice,char* text,FILE* outFile,struct TrieNode *root, long int size){
	char* part[MAX_THREAD_NUM];
	int lines[MAX_THREAD_NUM];
	int counter=1;
	int part_counter=0;
	int line_counter=1;


	for(int i=0;text[i];i++){
		if(text[i]=='\n'){
			line_counter++;
		}
	}

	for(int i=0;i<MAX_THREAD_NUM;i++){
		lines[i]=i*(line_counter/MAX_THREAD_NUM)+1;

		part[i]=malloc(sizeof(char)*size);
		if(part[i]==NULL){
			printf("memmory allocation failed. (line/thread driver)\n");
			exit(EXIT_FAILURE);
		}
		memset(part[i],0,sizeof(part[i]));
	}

	for(int i=0;text[i];i++){
		if(part_counter!=MAX_THREAD_NUM-1 && counter>=lines[part_counter+1]){
			strncat(part[part_counter],"\0",1);
			part_counter++;
		}

		strncat(part[part_counter],&text[i],1);				

		if(text[i]=='\n'){
			counter++;
		}
	}
	strncat(part[part_counter],"\0",1);

	if (choice==2){
		pthread_t tid[MAX_THREAD_NUM];
    	for(int i=0;i<MAX_THREAD_NUM;i++){
			struct thread_args *args;
			args = malloc(sizeof(struct thread_args));
			memset(args,0,sizeof(args));
			args->text=part[i];
			args->root=root;
			args->outFile=outFile;
			args->line=lines[i];
        	pthread_create(&tid[i],NULL,(void *)routine2,(void *)args);
    	}
    	for(int i=0;i<MAX_THREAD_NUM;i++){
        	pthread_join(tid[i],NULL);
    	}

	}else if(choice==3){
		pthread_t tid[MAX_THREAD_NUM];
    	for(int i=0;i<MAX_THREAD_NUM;i++){
			struct thread_args *args;
			args = malloc(sizeof(struct thread_args));
			memset(args,0,sizeof(args));
			args->text=part[i];
			args->root=root;
			args->outFile=outFile;
			args->line=lines[i];
        	pthread_create(&tid[i],NULL,(void *)routine1,(void *)args);
		
    	}
    	for(int i=0;i<MAX_THREAD_NUM;i++){
        	pthread_join(tid[i],NULL);
    	}
	}else{
		printf("Invalid input.\n");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char* argv[])
{
	FILE* inFile;
	FILE* outFile;
	struct TrieNode *root=malloc(sizeof(struct TrieNode));
	char* list[MAX_WORD_NUM];
	int list_size;
    char* text;
    long int size;
	int choice;

    if (argc<2){
        fputs("Not enough arguments.\n",stderr);
        // exit(EXIT_FAILURE);
		printf("Enter full address to the text file. (NO SPACES)\n");
		char filename[256]; 
		scanf("%s",filename);
		printf("%s\n", filename);
		inFile=fopen(filename,"rt");
		if(!inFile){
			fputs("Unable to open file.\n",stderr);
			exit(EXIT_FAILURE);
		}
    }else{
		inFile=fopen(argv[1],"rt");
		if(!inFile){
			fputs("Unable to open file.\n",stderr);
			exit(EXIT_FAILURE);
		}
	}
	if(root==NULL){
		fputs("memmory allocation failed. (root)\n",stderr);
		exit(EXIT_FAILURE);
	}

	fseek(inFile, 0L, SEEK_END);
	size = ftell(inFile);
	fseek(inFile,0L,SEEK_SET);
	text = malloc(size);

	if(text==NULL){
		fputs("memmory allocation failed. (text)\n",stderr);
		exit(EXIT_FAILURE);
	}

	while(true){
		char temp=(char)fgetc(inFile);
		if(temp==EOF){
			break;
		}
		strncat(text,&temp,1);
	}

	fclose(inFile);
	fputs("Succesfully opened and extracted text from the file.\n",stdout);

	for (int i=0;text[i];i++){
		text[i]=tolower(text[i]);
	}
	
	printf("Enter the number of words you wish to search in the file. (max=%d)\n",MAX_WORD_NUM);
	scanf("%d",&list_size);
	if(list_size>MAX_WORD_NUM){
		list_size=MAX_WORD_NUM;
	}
	for (int i=0;i<list_size;i++){
		printf("Enter a word:\n");
		list[i]=malloc(sizeof(char)*MAX_WORD_SIZE);
		if(list[i]==NULL){
			fputs("memmory allocation failed. (temp)\n",stderr);
			exit(EXIT_FAILURE);
		}
		scanf("%s",list[i]);
	}
	printf("OK.\n");
	
	create_trie_tree(root,list,list_size);
	usleep(500000);
	printf("tree created.\n");

	outFile=fopen("out.txt","wt");
	if(!outFile){
        fputs("Unable to open file.\n",stderr);
        exit(EXIT_FAILURE);
    }
	sem_init(&mutex,0,1);
	printf("select one of the options bellow:\n 1. Run without multi threading.\n 2. Multi threading using mutex lock.\n 3. Multi threading using semaphore.\n");
	scanf("%d",&choice);
	switch(choice){
		case 1 :
			printf("Searching...\n");
			usleep(1000000);
			word_search(root,text,outFile);
			fclose(outFile);
			printf("Search completed.\n");
			break;
		case 2 :
		case 3 :
			thread_driver(choice,text,outFile,root,size);
			break;
		default:
			thread_driver(choice,text,outFile,root,size);
			break;
	}
}
