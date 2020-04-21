/*
 * Course: CSE109
 * Semester and Year: Fall 2019
 * Assignment: prog3
 * File Purpose: this file the file for main()
 * Author: Huang, Surui
 * User ID: suh222
 */
#define BLOCKSIZE 16

#include <stdio.h>       /* for fprintf, sprintf */
#include <stdlib.h>      /* for exit */
#include <errno.h>       /* error handling */
#include <string.h>      /* for ? */
#include <curl/curl.h>   /* for curl */
#include <sys/stat.h>

#include "prog3_funcs.h"
/* compile with: makefile */


int main(int argc, char *argv[]) {
  char *ifile = NULL;               /* to point to input file name */
  FILE *fip = NULL;                 /* to point to opened input file */
  char *ofile = NULL;               /* to point to output file name */
  FILE *fop = NULL;                 /* to point to opened output file */
  char *kfile = NULL;               /* to point to key file name */
  FILE *fkp = NULL;                 /* to point to opened key file */
  unsigned char block[BLOCKSIZE];   /* to hold bytes read from input file */
  unsigned char key[BLOCKSIZE];     /* to hold key for encryption/decryption read from key file */

  char *server = NULL; 				/* to hold the server path */
  char *directory = NULL;			/* to hold the path of directory */
  char *userid = NULL;				/* to hold the userid */
  char *password = NULL;			/* to hold the password */
 // char *cfile = NULL;
  //FILE *calcfile = NULL; 			/* to hold the calculation file result */


  /* prog3 -i|--input <calculation file> -o|--output <results file> -k|--key <key file>
   * -s|--server -d|--directory <directory_on_server> -u|--userid <userID> -p|--password*/
  parseArgs(argc, argv, &ifile, &ofile, &kfile, &server, &directory, &userid, &password);    /* parse arguments and return pointers to filenames */

  /* securely download from the directory
   *
   * */
  ftpDownload(userid, password, server, directory, ifile);      /* call ftpDownload function and capture return code */


  /* open the downloaded file */
  fip = Fopen(ifile, "r");
  fop = Fopen(ofile, "w");
  fkp = Fopen(kfile, "r");

  readKey(key, BLOCKSIZE, fkp);                     /* read the key for encryption/decryption */
  encryptDecrypt(block, key, BLOCKSIZE, fip, fop);  /* encrypt/decrypt the input file and write to output file */


  /*close both files*/
  Fclose(fip);
  Fclose(fop);
  /*read the decrepted file as the input file with mode r*/
  fip = Fopen(ofile,"r");
  /* perform calculation and write the result to the calcfile */

  fop = Fopen(ifile, "w");

  /* do calculation, and save the result to the new file */
   calculateResult(fip, fop);
   //Fclose(fip);
   //Fclose(fop);
   Fclose(fip);
   Fclose(fop);
   fip = Fopen(ifile, "r");
   fop = Fopen(ofile, "w");
   fkp = Fopen(kfile, "r");
   readKey(key, BLOCKSIZE, fkp);
   encryptDecrypt(block, key, BLOCKSIZE, fip, fop);
   Fclose(fip);
   Fclose(fop);

   system("chmod 644 ofile");
   ftpUpload(userid, password, server, directory, ofile);

   if(ofile) free(ofile);
   if(ifile) free(ifile);
   if(kfile) free(kfile);
   if(userid) free(userid);
   if(server) free(server);
   if(directory) free(directory);
   if(password) free(password);
  return 0;
}
