#include "Count.h"
#include "prog4_funcs.h"

// No "using namespace std" (use scoping operator ::)
// No printf or fprintf statements
// No if-else statements, only switch statements
// Must use vector 
// Must use vector iterator
// Must use string
// Must define and use the overloaded insertion operator to print category counts
// Define a constructor which initializes all category counts
// Define a destructor which prints statistics (category counts and total)
// Must use 'this' pointer in destructor
// Only use std::endl at end of function (use '\n' elsewhere)

int main(int argc, char *argv[]) {
  parseArgs(argc, argv);

  Count count;

  parseFile(&count);

  return 0;
}
