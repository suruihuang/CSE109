/*
 * Course: CSE109
 * Semester and Year: Fall 2019
 * Assignment: Hash Table
 * Author: Surui Huang
 * User ID: suh222
 */

#include <stdio.h>       /* for fprintf and getline */
#include <string.h>      /* for strlen */
#include <stdlib.h>      /* for free */
#include <errno.h>       /* for errno, strerror */
#include <math.h>
#include "global_defs.h"
#include "prog2_funcs.h"

/* created global variable */
size_t INDEX = 0; /* initial index at the hashTable*/
int WORSTCHAINLENGTH = 0;
size_t BUCKETUSED =0;
int PERCENTAGEUSED = 0;
size_t TOTALBUCKETS = INITIALBUCKETS;


/* parseArg refers Dr.Erle's prog1 */
void parseArgs(int argc, char *argv[]) {
	/* if there is only one argument, display the help message*/
	if (argc == 1) {
		help();
	}

	/* if more than one argument is passed in the command line */
	for (int i = 1; i < argc; i++) {
		if (!strcasecmp(argv[i], "-h") || !strcasecmp (argv[i], "--help")) { /* !(int) any non-zero integer returns false, and !(0) returns true. */
			help();
		} else if (!strcasecmp(argv[i], "-i") || !strcasecmp (argv[i], "--input")) {
			if (++i >= argc) {
				bail(3, "No argument after '-i|--input'");
			}
			if(argc > 3){
				bail(2, "Input file has already been specified");
			}
		} else {
			bail(1, " Incorrect/unexpected argument entered, run without any arguments or with '-h|--help' for help");
		}
	}
}

/* bail function refers to Dr.Erle's prog1_funcs.c*/
void bail(int rc, const char *msg) {
  fprintf(stderr, "(%d) %s\n", rc, msg);
  exit(rc);
}

/* Fopen from Dr.Erle's prog1 */
FILE *Fopen(const char *path, const char *mode) {
  FILE *fp = NULL;
  char str[128];
  if ((fp = fopen(path, mode)) == NULL) {
    sprintf(str, "Unable to open %s with mode %s - %s", path, mode, strerror(errno));
    bail(10, str);
  } else {
    //if (*path) free((char *)path);  /* cast as pointer to non-const char */
  }  
  return fp; 
}

/* Fclose from Dr.Erle's prog1 */
void Fclose(FILE *fp) {
  char str[128];
  if (fp && fclose(fp)) {
    sprintf(str, "Unable to close file descriptor %d - %s", fileno(fp), strerror(errno));
    bail(13, str);
  }
}

/* Malloc from Dr.Erle's prog1 */
void *Malloc(size_t size) {
  char str[128];
  void *ptr;
  if ((ptr = malloc(size)) == NULL) {
    sprintf(str, "Could not allocate space - %s", strerror(errno));
    bail(99, str);
  }
  return ptr;
}

/* help message */
void help() {
  fprintf(stdout, "\n");
  fprintf(stdout, "prog2 creates, retrieves, updates, and deletes key-value pairs in a hash table. \n");
  fprintf(stdout, "Keys can be any integer in the range [0, 18446744073709551615] and values can be\n");
  fprintf(stdout, "any integer in the range [-2147483648, 2147483647]. Commands to perform the above\n");
  fprintf(stdout, "actions may be specified in the input text file as follows.\n\n");
  fprintf(stdout, "# format is: action key [value] [# comment]\n");
  fprintf(stdout, "# action is one of the following characters: c r u d\n");
  fprintf(stdout, "c key value  # if key already in table, value will be updated and a warning printed\n");
  fprintf(stdout, "r key        # if key not in the table, a warning will be printed, otherwise value\n");
  fprintf(stdout, "u key value  # if key not in the table, value will be stored and a warning printed\n");
  fprintf(stdout, "d key        # if key not in the table, a warning will be printed\n\n");
  fprintf(stdout, "The table auto-resizes to maintain \"optimum \" performance. Run program as follows.\n\n");
  fprintf(stdout, "prog2 -i|--input <input file>\n");
  fprintf(stdout, "\t or\nprog2 [-h|--help]\n\n");
  exit(0);
}

/* in: pointer to current hash table (if there is one), number of initial or new buckets; return: pointer to new hash table */
/* if the pointer is null, then create a new hash table */
/* if the pointer is not null, then it's a resize */
Node **createHashTable(Node **hashTable, size_t bucket) {
	Node **head = NULL;

	/* if the hashTable passed in the function D.N.E */
	if(hashTable == NULL) {
		//initial bucket * the size of a pointer, which is 8
		head =  Malloc(bucket * sizeof(Node *));
		for(size_t i = 0; i < bucket; i++){
			head[i] = NULL;
		}
		hashTable = head;

	}else{
		/* for resize purpose, malloc size times 1.5 for new hashTable*/
		//initial new size  * the size of a pointer, which is 8
		TOTALBUCKETS =  TOTALBUCKETS + (TOTALBUCKETS * 50 + 99)/100;
		head = Malloc(TOTALBUCKETS * sizeof(Node *));
		for(size_t i = 0; i < TOTALBUCKETS; i++){
			head[i] = NULL;
		}

		copyHashTable(head, hashTable, bucket);
	}

	return head;
}

/* in: pointer to new hash table, pointer to current hash table, size of current hash table */
void copyHashTable(Node **newHash, Node **oldHash, size_t bucket){
	uint64_t tmpKey = 0;
	int tmpVal = 0;
	Node *ptr = NULL;
	//loop through the old hashTable, pop out each key and value and put it to the new hashTable
	for(size_t i = 0; i < bucket; i++ ){
		ptr = oldHash[i];
		while(ptr != NULL){
			//get the value in old hashTable
			tmpKey = ptr->key;
			tmpVal = ptr->value;
			//put these value into the new hashTable
			create(newHash, tmpKey, tmpVal);
			//shift to the next pointer
			ptr = ptr->next;
		}
	}
	oldHash = newHash;

}

/* in: 8-byte key; return: 4 byte hash */
uint32_t hash(uint64_t key){
	uint32_t hash = 5381;
	 uint8_t * ptr = &key;

	  //hash = (uint32_t)key;                        /* this is temporary; you need to replace this with an actual, iterative, hash function */
		for (int i = 0; i < 8; i++) {
			hash = ((hash << 5) + hash) ^ *ptr;
			ptr++;
		}


	  return hash;
}
/* in: pointer to hash table, pointer to commands file; return: pointer to new hash table */
/* reference to Dr.Erle's ParseArg program */
Node **runHashCommands(Node **hashTable, FILE *cmdFile){
	/* variables for file read */
	FILE *fp = cmdFile;
	char *line = NULL; //store each line and used for sscanf
	size_t len = 0;
	ssize_t bitRead;
	/* variables for sscanf */
	char action = Malloc(sizeof(char *));
	uint64_t key = 0;
	size_t value = 0;
	int count = 0;
	/* variables for return code from the functions */
	int returnCode = 0;
	int valueToRead = NULL;
	Node **newHashTable = NULL;
	/* read by lines */
	while((bitRead = getline(&line, &len, fp)) != -1) {
		/* update the bucket size before running the cmd */

		count = sscanf(line, "%s %ld %ld", &action, &key, &value);
		//skip the first comment line
		if (count <= 3){
			//call the create method
			if(action == 'c'){
				returnCode = create(hashTable, key, value);

				if(returnCode != 0){
					fprintf(stderr, "Warning: update was occurred: %ld %ld", key, value);
				}
			} else if(action == 'd'){
				returnCode = delete(hashTable, key);

				if(returnCode != 0){
					fprintf(stderr, "Warning: fail to delete: %ld %ld", key, value);
				}
			} else if(action == 'u'){
				returnCode = update(hashTable, key, value);

				if(returnCode != 0){
					fprintf(stderr, "Warning: create occurred: %ld %ld", key, value);
				}
			} else if(action == 'r') {
				returnCode = read(hashTable, key, &valueToRead);

				if(returnCode != 0){
					fprintf(stderr, "fail to read: %ld %ld", key, value);
				}
				if(returnCode == 0){
					fprintf(stdout, "key %ld was found as: %d  \n", key, valueToRead);
				}
			}
		}
		getHashTableStats(hashTable);
				/* if the bucket used has exceed 75% */
			if((PERCENTAGEUSED > PERCENTAGETHRESHOLD) || (WORSTCHAINLENGTH > CHAININGTHRESHOLD)){
			//print status before resizing
				fprintf(stdout, "Stats before resizing: ");
				printHashTableStats();
				newHashTable = createHashTable(hashTable, TOTALBUCKETS);
				hashTable = newHashTable;
				WORSTCHAINLENGTH = 0;
				getHashTableStats(hashTable);
				fprintf(stdout, "Stats after resizing: ");
				printHashTableStats();
			}
	}
	/*update the hashTable */
	getHashTableStats(hashTable);
	fprintf(stdout, "Stats at the end: ");
	printHashTableStats();


	return hashTable;
}     

/* in: pointer to hash table, key, value; return 0 for success and 1 when update occurred  */
int create(Node **hashTable, uint64_t key, int value){
	Node *head = Malloc(sizeof(Node));
	head->value = value;
	head->key = key;
	head->next = NULL;
	INDEX = (hash(key)) % TOTALBUCKETS; //Find the index of the key
	Node *ptr = hashTable[INDEX];
	int count = 0;
	if(ptr != NULL){
		while(ptr->next != NULL){
			if(ptr->key == key){
				update(hashTable, key, value);
				return 1;
			}
			count++;
			ptr = ptr->next;
		}
		if(ptr->key == key){
			update(hashTable, key, value);
			return 1;
		}
		ptr->next = head;
	}else{
		hashTable[INDEX] = head;
	}
	return 0;
}

/* in: pointer to hash table, key, value; return 0 for success and 1 when create occurred  */
int update(Node **hashTable, uint64_t key, int value){
	INDEX = (hash(key)) % TOTALBUCKETS;
	Node *ptr = hashTable[INDEX];
	int count = 0;
	if(ptr == NULL){
		create(hashTable, key, value);
		return 1;
	} else{
		//if the next pointer exist
		while(ptr != NULL){
			/* if the key matches */
			if(ptr->key == key){
				count++;
				ptr->value = value;
				return 0;
			}
			ptr = ptr->next;
		}
	}
	/* no key is found */
	if(count == 0){
		create(hashTable,key,value);
		return 1;
	}
	return 0;
}

/*
 * in: pointer to hash table, key, pointer to value to be updated; return 0 for success and 2 when error
 * from piazza:
 * Read will search for key in the hash table and return the value.
 * In order to return the key outside your read() function,
 * you need to dereference the value pointer passed to it.
 * Hence the "int *value". Recall swap.c...
 */
int read(Node **hashTable, uint64_t key, int *value){
	INDEX = (hash(key)) % TOTALBUCKETS;
	Node *ptr = hashTable[INDEX];
	if (ptr == NULL){
		//send out error
		return 2;
	}else{
		//if the next pointer exist
		while(ptr != NULL){
			/* if the key matches */
			if(ptr->key == key ){
				*value = (ptr->value);
				return 0;
			}
			ptr = ptr->next;
		}
		return 2;//no key was found
	}
	return 0;
}

/* in: pointer to hash table, key; return 0 for success and 2 when not successful  */
int delete(Node **hashTable, uint64_t key){
	INDEX = (hash(key)) % TOTALBUCKETS;
	Node *prev = hashTable[INDEX];
	Node *ptr = prev->next;

	//if the current index is NULL
	if (prev == NULL ){
		return 2;
	}
	//if the current ptr is the only ptr
	if (prev->key == key){
		hashTable[INDEX] = prev->next;
		return 0;
	}
	while(ptr != NULL){
		if(ptr->key == key){
			prev = ptr->next;
			if(ptr){
				free(ptr);
			}
			return 0;
		}
		prev = ptr;
		ptr = ptr->next;
	}
	return 0;
}

/*
 * in: pointer to hash table
 * From piazza
 *  This function prints out some global variables and does not need a pointer to the hash table
 */
void getHashTableStats(Node **hashTable) {
	int chainCount = 0;
	int bucketCount = 0;
	int maxChain = 0;
	Node *ptr = NULL;
	for(size_t i = 0; i < TOTALBUCKETS ; i++){
		chainCount = 0;
		ptr = hashTable[i];
		while(ptr != NULL){
			chainCount++;
			ptr = ptr->next;
		}

		if(hashTable[i] != NULL){
			bucketCount++;
		}
		if(chainCount > maxChain){
			maxChain = chainCount;
		}
	}
	if((maxChain > WORSTCHAINLENGTH) & (bucketCount != 0)){
		WORSTCHAINLENGTH = maxChain;
	}
	if(bucketCount == 0){
		WORSTCHAINLENGTH = 0;
	}
	BUCKETUSED = bucketCount;
	PERCENTAGEUSED =100 * BUCKETUSED / TOTALBUCKETS;
}

/*
 * From piazza
 *  This function prints out some global variables and does not need a pointer to the hash table
 */
void printHashTableStats(){
	fprintf(stdout, "%ld buckets, ", TOTALBUCKETS);
	fprintf(stdout, " %d%% of buckets used, ", PERCENTAGEUSED);
	fprintf(stdout, "worst chain length = %d \n", WORSTCHAINLENGTH);

}              

/* in: pointer to hash table */         
void freeHashTable(Node **hashTable) {
	Node *ptr = NULL;
	Node *fptr = NULL; //ptr to be freed
	for(size_t i = 0; i < TOTALBUCKETS; i++){
		ptr = hashTable[i];
		while(ptr != NULL){
			fptr = ptr;
			ptr = ptr->next;
			if(fptr){
				free(fptr);
			}
		}
	}
}                     
