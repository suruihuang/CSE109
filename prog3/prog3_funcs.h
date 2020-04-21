/*
 * Course: CSE109
 * Semester and Year: Fall 2019
 * Assignment: prog3
 * File Purpose: this is the header file
 * Author: Huang, Surui
 * User ID: suh222
 */
#ifndef PROG3_FUNCS
#define PROG3_FUNCS

/* Refer to prog1_funcs.h and curlFTP.c */
void   parseArgs(int, char *[], char **, char **, char **, char **, char **, char**, char**);     /* in: argc, argv; out: ifile, ofile, kfile, server, directory, userid, password */
void  *Malloc(size_t);                                          /* in: bytes to allocated; return: void ptr to allocated memory */
FILE  *Fopen(const char *, const char *);                       /* in: path/file, mode; return: file ptr */
void   readKey(unsigned char *, size_t, FILE *);                         /* in: buffer ptr, key length, key file ptr */
void   encryptDecrypt(unsigned char *, unsigned char *, size_t, FILE *, FILE *);  /* in: buffer ptr, key ptr, buffer size, input file ptr, output file ptr */
size_t Fread(void *, size_t, size_t, FILE *);                   /* in: buffer ptr, buffer size, num buffers, input file ptr; return: bytes read */
size_t Fwrite(const void *, size_t, size_t, FILE *);            /* in: buffer ptr, buffer size, num buffers, output file ptr; return: bytes written */
void   Fclose(FILE *);                                          /* in: file ptr */
void   bail(int, const char *);                                 /* in: rc to exit with, ptr to message to print */
void   help();
int     ftpDownload(char *, char *, char *, char *,char *);
size_t  ftpWrite(void *, size_t, size_t, void *);

void calculateResult(FILE *, FILE *);					/* in:buffer ptr, buffer size , input file ptr, output file ptr*/
int     ftpUpload(char *, char *, char *, char *,char *);
size_t  ftpRead(void *, size_t, size_t, void *);
#endif
