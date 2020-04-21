/*
 * Course: CSE109
 * Semester and Year: Fall 2019
 * Assignment: cURL
 * File Purpose: hold FTP download and upload functions
 * Author: Surui Huang
 * User ID: suh222
 */

#include <stdio.h>       /* for fprintf, etc. */
#include <stdlib.h>      /* for exit */
#include <string.h>      /* for strerror */
#include <errno.h>       /* for errno */
#include <curl/curl.h>   /* for curl, baby! */

/* link using -lcurl */

/* FYI:  running the following will give you  the / on the end results in directory listing */
/* curl sftp://merle@sunlab.cse.lehigh.edu/home/<userid>/  */

/* These would probably be in prog3_funcs.c */
       void    bail(int, const char *);
       void    help(void);
       FILE   *Fopen(const char *, const char *);
       size_t  Fwrite(const void *, size_t, size_t, FILE *);
       void    Fclose(FILE *);

/* These functions and the struct object are unique to this .c file */
static int     ftpDownload(char *, char *, char *, char *);
static size_t  ftpWrite(void *, size_t, size_t, void *);

struct ftpfile {
  const char *filename;   /* name to store the file as */
  FILE *stream;           /* file pointer */
};
typedef struct ftpfile ftpFile_t;


/* remove main function after testing */
int main(int argc, char *argv[]) {
  char str[256];
  int rc = 0;
  
  if (argc == 5) {
    rc = ftpDownload(argv[1], argv[2], argv[3], argv[4]);      /* call ftpDownload function and capture return code */
    if (rc) {                                                  /* bail if transfer was unsuccessful */
      sprintf(str, "Unable to download %s from %s (curl rc = %d (%s))", argv[4], argv[3], rc, curl_easy_strerror(rc));
      bail(15, str);
    }
  } else {                                                     /* if 4 arguments were not passed, call help */
    help();
    exit(0);
  }

  return rc;
}


static int ftpDownload(char *userid, char *password, char *server, char *pathname) {
  char str[128];                                               /* character string for setting curl settings, call values, or bail messages */
  CURLcode crc;                                                /* variable to hold CURL function recturn codes */
  CURL *cp = NULL;                                             /* pointer/handle to curl session/transfer */
  ftpFile_t FTPfile = { "downloadedFile", NULL };              /* declare and define an ftpFile_t object, you can change filename or make it a variable */

  curl_global_init(CURL_GLOBAL_ALL);                           /* initializes curl global state; setup and initializes underlying libraries */

  cp = curl_easy_init();                    	               /* initialize pointer/handle for curl session/transfer */

  if (cp) {                                                    /* check if curl handle creation was successful, and if so, proceed */
    sprintf(str, "%s:%s", userid, password);
    curl_easy_setopt(cp, CURLOPT_USERPWD, str );               /* set username and password */
    sprintf(str, "sftp://%s/%s", server, pathname);
    curl_easy_setopt(cp, CURLOPT_URL, str);                    /* set protocol, server, directory, and file */

    curl_easy_setopt(cp, CURLOPT_WRITEFUNCTION, ftpWrite);     /* declare the function we want called after the connection succeeds */

    curl_easy_setopt(cp, CURLOPT_WRITEDATA, &FTPfile);        /* set a pointer to our struct to pass to the callback */

    curl_easy_setopt(cp, CURLOPT_VERBOSE, 1L);                 /* switch on full protocol/debug output */

    crc = curl_easy_perform(cp);                               /* perform the curl call and capture the return code*/

    curl_easy_cleanup(cp);                                     /* cleanup local session/transfer */

  } else {                                                     /* bail in the unlikely event curl handle creation failed */
    bail(99, "Initialization of curl session/transfer handle was unsuccessful");
  }

  Fclose(FTPfile.stream);                                        	    /* close the local file */

  curl_global_cleanup();                                                /* free and clean up associated global resources init call allocated */

  return (int)crc;                                             /* return curl return code */
}

/* You don't need to change this */
static size_t ftpWrite(void *buffer, size_t size, size_t nmemb, void *stream) {
  ftpFile_t *oFile = (ftpFile_t *)stream;
  if(!oFile->stream)
    oFile->stream = Fopen(oFile->filename, "w");  /* open file for writing */
  return Fwrite(buffer, size, nmemb, oFile->stream);
}

/* following  were cut-and-pasted from earlier prog*_funcs.c file(s) */

void help(void) {
  fprintf(stdout, "\nUsage: curlFTP <userid> <password> <server> <fully-qualified filename>\n\n");
}

void bail(int rc, const char *msg) {    
  fprintf(stderr, "(%d) %s\n", rc, msg);
  exit(rc);
}

FILE *Fopen(const char *path, const char *mode) {
  FILE *fp = NULL;
  char str[128];
  if ((fp = fopen(path, mode)) == NULL) {
    sprintf(str, "Unable to open %s with mode %s - %s", path, mode, strerror(errno));
    bail(10, str);
  }   
  return fp; 
} 

void Fclose(FILE *fp) {
  char str[128];
  if (fp && fclose(fp)) {
    sprintf(str, "Unable to close file descriptor %d - %s", fileno(fp), strerror(errno));
    bail(13, str);
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

