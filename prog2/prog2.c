/*
 *
 * Main source code file for hash table program
 * CSE109 - Spring 2019
 *
 */

#include <stdio.h>          /* for FILE */
#include "global_defs.h"    /* for node structure and typedef Node */
#include "prog2_funcs.h"    /* for supporting functions */

int main(int argc, char *argv[]) {
  Node **hashTable = NULL;
  FILE *cmdFile = NULL;

  parseArgs(argc, argv);

  cmdFile = Fopen(argv[2], "r");

  hashTable = createHashTable(hashTable, INITIALBUCKETS);

  hashTable = runHashCommands(hashTable, cmdFile);

  freeHashTable(hashTable);

  Fclose(cmdFile);
  
  return 0;
}
