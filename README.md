# CSE109
 This contains some course works in CSE109
# Lab1: 
# Lab2:
  Learning Objectives for Student
    A. Hash functions and hash tables
    B. Structures and accessing structure members
    C. Pointers and arrays of pointers
  II. Description of Program and Program Requirements
    A. Write a program whose executable name will be prog2 and which conforms to the following
    “specification” (which is actually the help message).
    prog2 prog2 creates, retrieves, updates, and deletes key-value pairs in a hash table.
    The table auto-resizes to maintain “optimum” performance. Run program as follows.
      prog2 -i|--input <input file>
      or
      prog2 [-h|--help]
    B. Use a makefile to manage the selective compilation of your relocatable and executable objects
    files. You will need to list global_defs.h, prog2_funcs.h, and prog2_funcs.c as prerequisites for
    prog2_funcs.o. And you will need to list global_defs.h, prog2_funcs.h, and prog2.c as
    prerequisites for prog2.o. You may choose to pass -DDEBUG as a parameter to gcc to print
    additional information during debug. This is fine, so long as it is obvious this is NOT your
    primary method for debug... which should be gdb. When we compile for grading, we will not
    compile with -DDEBUG, and it is fine if your submitted code has statements like “if (DEBUG) {
    ... }”.
    C. During development, you may find it beneficial to work with a smaller INITIALBUCKETS value.
    To do so, simply change the “#define INITIALBUCKETS 100” to a smaller value and
    recompile. Same goes for CHAININGTHRESHOLD, PERCENTAGETHRESHOLD, and
    RESIZEPERCENTAGE.
    D. Exit with the following return code and an appropriate message to stderr under the following
    conditions. For items marked with ‘*’, print the errno value and the strerror(errno)
    message (still to stderr). Except for the conditions specified below, the return code is 0.
      ● 1 - Incorrect/unexpected argument entered
      ● 2 - Input file already specified
      ● 3 - No argument after '-i|--input'
      ● 10 - Unable to open file *
      ● 13 - Unable to close file *
      ● 99 - Could not allocate space *
