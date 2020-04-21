#include <stdio.h>       /* for fprintf and getline */
#include <string.h>      /* for strlen */
#include <stdlib.h>      /* for free */
#include <errno.h>       /* for errno, strerror */
#include "prog1_funcs.h"

#ifdef DEBUG
#define DEBUG 1
#else
#define DEBUG 0
#endif

void parseArgs(int argc, char *argv[], char **ifile, char **ofile, char **kfile) {
  if (argc == 1) help();
  for (int i = 1; i < argc; i++) {
    if        (!strcasecmp(argv[i], "-h") || !strcasecmp (argv[i], "--help")) {
      help();
    } else if (!strcasecmp(argv[i], "-i") || !strcasecmp (argv[i], "--input")) {
      if (++i < argc) {
        *ifile = (char *)Malloc(strlen(argv[i]) + 1);
        strcpy(*ifile, argv[i]);
      } else {
        bail(3, "No argument after '-i|--input'");
      }
    } else if (!strcasecmp(argv[i], "-o") || !strcasecmp (argv[i], "--output")) {
      if (++i < argc) {
        *ofile = (char *)Malloc(strlen(argv[i]) + 1);
        strcpy(*ofile, argv[i]);
      } else {
        bail(4, "No argument after '-o|--output'");
      }
    } else if (!strcasecmp(argv[i], "-k") || !strcasecmp (argv[i], "--key")) {
      if (++i < argc) {
        *kfile = (char *)Malloc(strlen(argv[i]) + 1);
        strcpy(*kfile, argv[i]);
      } else {
        bail(5, "No argument after '-k|--key'");
      }
    } else {
      bail(1, "Incorrect/unexpected argument entered, run without any arguments or with '-h|--help' for help");
    }
  }
  if (*ifile == NULL || *ofile == NULL || *kfile == NULL)
    bail(2, "One or more arguments not specified");
}

void *Malloc(size_t size) {
  char str[128];
  void *ptr;
  if ((ptr = malloc(size)) == NULL) {
    sprintf(str, "Could not allocate space - %s", strerror(errno));
    bail(99, str);
  }
  return ptr;
}

FILE *Fopen(const char *path, const char *mode) {
  FILE *fp = NULL;
  char str[128];
  if ((fp = fopen(path, mode)) == NULL) {
    sprintf(str, "Unable to open %s with mode %s - %s", path, mode, strerror(errno));
    bail(10, str);
  } else {
    if (*path) free((char *)path);  /* cast as pointer to non-const char */
  }  
  return fp; 
}

void readKey(unsigned char *key, size_t block_size, FILE *fp) {
  size_t items_read = 0;
  char str[128];          /* for error msg */
  if ((items_read = Fread(key, sizeof(char), block_size, fp)) < block_size) {
    sprintf(str, "Key is fewer bytes than expected (%zu vs. %zu)", items_read, block_size);
    bail(20, str);
  }
}

size_t Fread(void *ptr, size_t item_size, size_t num_items, FILE *fp) {
  size_t items_read = 0;
  char str[128];          /* for error msg */

  /* The function fread() reads num_items of data, each item_size bytes long, from the stream pointed to 
   * by fp, storing them at the location given by ptr. */
  if ((items_read = fread(ptr, item_size, num_items, fp)) == 0) {  /* getting 0 may not be an error */
    if (!feof(fp) && ferror(fp)) {   /* this means we did not reach EOF but encountered an error */
      sprintf(str, "Encountered error while reading from file descriptor %d - %s", fileno(fp), strerror(errno));
      bail(11, str);
    }   
  }   
  return items_read;
}

void encryptDecrypt(unsigned char *block, unsigned char *key, size_t block_size, FILE *fip, FILE *fop) {
  size_t items_read;
  size_t items_written;
  while (!feof(fip)) {
    items_written = 0;
    if ((items_read = Fread(block, sizeof(char), block_size, fip))) {
      for (size_t i = 0; i < items_read; i++) {
        if (DEBUG) printf("%#04X ^ %#04X = ", block[i], key[i]);
        block[i] ^= key[i];
        if (DEBUG) printf("%#04X\n", block[i]);
      }   
      items_written = Fwrite(block, sizeof(char), items_read, fop);
    }     
    if (items_written != items_read)
      bail(30, "Unable to write all items read");
  }
}

size_t Fwrite(const void *ptr, size_t item_size, size_t num_items, FILE *fp) { 
  size_t items_written = 0;
  char str[128];

  if ((items_written = fwrite(ptr, item_size, num_items, fp)) == 0) {
    if (ferror(fp)) {
      sprintf(str, "Encountered error while writing to file descriptor %d - %s", fileno(fp), strerror(errno));
      bail(12, str);
    }   
  }
  return items_written;
}

void Fclose(FILE *fp) {
  char str[128];
  if (fp && fclose(fp)) {
    sprintf(str, "Unable to close file descriptor %d - %s", fileno(fp), strerror(errno));
    bail(13, str);
  }
}

void bail(int rc, const char *msg) {
  fprintf(stderr, "(%d) %s\n", rc, msg);
  exit(rc);
}

void help() {
  fprintf(stdout, "\n");
  fprintf(stdout, "prog1 performs an XOR-based encryption/decryption on the specified input file using the\n");
  fprintf(stdout, "specified key file to produce the specified output file. The program is called as follows.\n\n");
  fprintf(stdout, "prog1 -i|--input <input file> -o|--output <output file> -k|--key <key file>\n");
  fprintf(stdout, "\tor\nprog1 [-h|--help]\n\n");
  exit(0);
}

