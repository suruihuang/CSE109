/*
 * Course: CSE109
 * Semester and Year: Fall 2019
 * Assignment: prog3
 * File Purpose: this file contains the functions; serve as the prog3_funcs.c
 * Author: Huang, Surui
 * User ID: suh222
 */

#include <stdio.h>       /* for fprintf and getline */
#include <string.h>      /* for strlen */
#include <stdlib.h>      /* for free */
#include <errno.h>       /* for errno, strerror */
#include <curl/curl.h>   /* for curl */
#include <sys/stat.h>



#ifdef DEBUG
#define DEBUG 1
#else
#define DEBUG 0
#endif

/* structures and functions that are specific for curl functions */
/* These functions and the struct object are unique to this .c file */



struct ftpfile {
  const char *filename;   /* name to store the file as */
  FILE *stream;           /* file pointer */
};
typedef struct ftpfile ftpFile_t;


void bail(int rc, const char *msg) {
  fprintf(stderr, "(%d) %s\n", rc, msg);
  exit(rc);
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

void help(void) {
	fprintf(stdout, "\nprog3 securely downloads an encrypted calculation file, decrypts the calculation file using a\n");
	fprintf(stdout, "unique key, performs the arithmetic operation in the calculation file, generates a results file \n");
	fprintf(stdout, "containing the arithmetic operation and its result, encrypts the results file, and securely uploads\n");
	fprintf(stdout, "the encrypted results file. Run program as follows.\n\n");
	fprintf(stdout, "prog3 -i|--input <calculation file> -o|--output <results file> -k|--key <key file> -s|--server \n");
	fprintf(stdout, "<server_name> -d|--directory <directory_on_server> -u|--userid <userID> -p|--password \n");
	fprintf(stdout, "<password>\n");
	fprintf(stdout, "\t or\n");
	fprintf(stdout, "prog3 [-h|--help]\n");
}

/* take the command line arguments and assign the pointers to the needed variables */
void parseArgs(int argc, char *argv[], char **ifile, char **ofile, char **kfile, char **server, char **directory, char**userid, char**password){
	if(argc == 1){
		help();
	}
	for(int i = 1; i < argc; i++){
		if(!strcasecmp(argv[i], "-h") || !strcasecmp(argv[i], "--help")){
			help();
		}
		else if(!strcasecmp(argv[i], "-i") || !strcasecmp(argv[i], "--input")){
			if(++i < argc) {
				*ifile = (char *)Malloc(strlen(argv[i]) + 1);
				strcpy(*ifile, argv[i]);
			} else{
				bail(3, "No argument after '-i|--input'");
			}
		}
		else if(!strcasecmp(argv[i], "-o") || !strcasecmp (argv[i], "--output")) {
			if (++i < argc){
		        *ofile = (char *)Malloc(strlen(argv[i]) + 1);
		        strcpy(*ofile, argv[i]);
			}else {
		        bail(4, "No argument after '-o|--output'");
		    }
		}
		else if(!strcasecmp(argv[i], "-k") || !strcasecmp(argv[i], "--key")){
			if (++i < argc){
				*kfile = (char *)Malloc(strlen(argv[i]) + 1);
		        strcpy(*kfile, argv[i]);
			}else {
		        bail(5, "No argument after '-k|--key'");
			}
		}
		else if(!strcasecmp(argv[i], "-s") || !strcasecmp(argv[i], "--server")){
			if (++i < argc){
				*server = (char *)Malloc(strlen(argv[i]) + 1);
		        strcpy(*server, argv[i]);
			}else {
		        bail(6, "No argument after '-s|--server'");
			}
		}
		else if(!strcasecmp(argv[i], "-d") || !strcasecmp(argv[i], "--directory")){
			if (++i < argc){
				*directory = (char *)Malloc(strlen(argv[i]) + 1);
				strcpy(*directory, argv[i]);
			}else {
				bail(7, "No argument after '-d|--directory'");
			}
		}
		else if(!strcasecmp(argv[i], "-u") || !strcasecmp(argv[i], "--userid")){
			if (++i < argc){
				*userid = (char *)Malloc(strlen(argv[i]) + 1);
				strcpy(*userid, argv[i]);
			}else {
				bail(8, "No argument after '-u|--userid'");
			}
		}
		else if(!strcasecmp(argv[i], "-p") || !strcasecmp(argv[i], "--password")){
			if(++i < argc){
				*password = (char *)Malloc(strlen(argv[i]) +1);
				strcpy(*password, argv[i]);
			}else{
				bail(9, "No argument after '-p|--password'");
			}
		}
		else{
			bail(1, "Incorrect/unexpected argument entered, run without any arguments or with '-h|--help' for help");
		}
	}
	if (*ifile == NULL || *ofile == NULL || *kfile == NULL || *server == NULL || *directory ==NULL || *userid ==NULL || *password ==NULL){
		bail(2, "One or more arguments not specified");
	}
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

size_t Fwrite(const void *ptr, size_t item_size, size_t num_items, FILE *fp) {
  size_t items_written = 0;
  char str[128];

  if ((items_written = fwrite(ptr, item_size, num_items, fp)) == 0) {
    if (ferror(fp)) {
      sprintf(str, "Error writing file * %d - %s", fileno(fp), strerror(errno));
      bail(12, str);
    }
  }
  return items_written;
}

void Fclose(FILE *fp){
	char str[128];
	if (fp && fclose(fp)) {
		sprintf(str, "Unable to close file * %d - %s", fileno(fp), strerror(errno));
		bail(13, str);
	}
}

size_t ftpWrite(void *buffer, size_t size, size_t nmemb, void *stream) {
  ftpFile_t *oFile = (ftpFile_t *)stream;
  if(!oFile->stream)
    oFile->stream = Fopen(oFile->filename, "w");  /* open file for writing */
  return Fwrite(buffer, size, nmemb, oFile->stream);
}

/* download the file from server directory
 * refer to curlFTP lab
 */
int ftpDownload(char *userid, char *password, char *server, char *directory, char *ifile) {
  char str[128];                                               /* character string for setting curl settings, call values, or bail messages */
  CURLcode crc = 0;                                                /* variable to hold CURL function recturn codes */
  CURL *cp = NULL;                                             /* pointer/handle to curl session/transfer */
  ftpFile_t FTPfile = { "suh222.enc", NULL };              /* declare and define an ftpFile_t object, you can change filename or make it a variable */

  curl_global_init(CURL_GLOBAL_ALL);                           /* initializes curl global state; setup and initializes underlying libraries */

  cp = curl_easy_init();                    	               /* initialize pointer/handle for curl session/transfer */

  if (cp) {                                                    /* check if curl handle creation was successful, and if so, proceed */
    sprintf(str, "%s:%s", userid, password);
    curl_easy_setopt(cp, CURLOPT_USERPWD, str );               /* set username and password */
    sprintf(str, "sftp://%s/%s/%s", server, directory, ifile);
    curl_easy_setopt(cp, CURLOPT_URL, str);                    /* set protocol, server, directory, and file */

    curl_easy_setopt(cp, CURLOPT_WRITEFUNCTION, ftpWrite);     /* declare the function we want called after the connection succeeds */

    curl_easy_setopt(cp, CURLOPT_WRITEDATA, &FTPfile);        /* set a pointer to our struct to pass to the callback */

    curl_easy_setopt(cp, CURLOPT_VERBOSE, 1L);                 /* switch on full protocol/debug output */

    crc = curl_easy_perform(cp);                               /* perform the curl call and capture the return code*/
    if(crc == 67){
    	sprintf(str, "Authentication unsuccessful ** - %s", curl_easy_strerror(crc));
    	bail(14, str);
   	}

    curl_easy_cleanup(cp);                                     /* cleanup local session/transfer */

  } else {                                                     /* bail in the unlikely event curl handle creation failed */
	  sprintf(str, "Download unsuccessful **- %s", curl_easy_strerror(crc));
	  bail(15, str);
  }

  Fclose(FTPfile.stream);                                        	    /* close the local file */

  curl_global_cleanup();                                                /* free and clean up associated global resources init call allocated */

  return (int)crc;                                             /* return curl return code */
}

size_t Fread(void *ptr, size_t item_size, size_t num_items, FILE *fp) {
  size_t items_read = 0;
  char str[128];          /* for error msg */

  /* The function fread() reads num_items of data, each item_size bytes long, from the stream pointed to
   * by fp, storing them at the location given by ptr. */
  if ((items_read = fread(ptr, item_size, num_items, fp)) == 0) {  /* getting 0 may not be an error */
    if (!feof(fp) && ferror(fp)) {   /* this means we did not reach EOF but encountered an error */
      sprintf(str, "Error reading file * %d - %s", fileno(fp), strerror(errno));
      bail(11, str);
    }
  }
  return items_read;
}

void readKey(unsigned char *key, size_t block_size, FILE *fp) {
  size_t items_read = 0;
  char str[128];          /* for error msg */
  if ((items_read = Fread(key, sizeof(char), block_size, fp)) < block_size) {
    sprintf(str, "Key is fewer bytes than expected (%zu vs. %zu)", items_read, block_size);
    bail(20, str);
  }
}

void encryptDecrypt(unsigned char *block, unsigned char *key, size_t block_size, FILE *fip, FILE *fop) {
  size_t items_read;
  size_t items_written;
  while (!feof(fip)) {
    items_written = 0;
    if ((items_read = Fread(block, sizeof(char), block_size, fip))) {
      for (size_t i = 0; i < items_read; i++) {
        block[i] ^= key[i];
      }
      items_written = Fwrite(block, sizeof(char), items_read, fop);
    }
    if (items_written != items_read)
      bail(30, "Unable to write all items read");
  }

}

void calculateResult(FILE *fip, FILE *fop){
	size_t items_read;
	//size_t items_written;
	long int first_num;
	long int second_num;
	char *operator = (char *)Malloc(sizeof(char *));
	long int result;
	char tmp[256];

	char *mult = (char *)"*";
	char *add = (char *)"+";
	char *minus = (char *)"-";
	char *divide =(char *) "/";

	while((int)(items_read= fscanf(fip, "%ld %s %ld", &first_num, operator, &second_num)) != (-1)){
		if(!strcmp(operator, mult)){
			result = first_num * second_num;
		}
		else if(!strcmp(operator, add)){
			result = first_num + second_num;
		}
		else if(!strcmp(operator, minus)){
			result = first_num - second_num;
		}
		else if(!strcmp(operator, divide)){
			result = first_num / second_num;
		}
	}
	/* write the result to a file */
	sprintf(tmp,"%ld", result);
	Fwrite(tmp, sizeof(char), strlen(tmp), fop);
	if(operator){
		free(operator);
	}
}

size_t ftpRead (void *ptr, size_t size, size_t nmemb, void *stream){

	  size_t retcode = Fread(ptr, size, nmemb, stream);

	  return (curl_off_t)retcode;;
}

/* ftp upload
 * refer to curl.haxx.se/libcurl/c/ftpupload.html */

int	ftpUpload(char *userid, char *password, char *server, char *directory, char *ifile){
	CURL *cp;				/* curl ptr*/
	CURLcode crc = 0;			/* curl return code */
	char str[128];
	//file structure
	FILE *hd_src;
	 hd_src = Fopen(ifile, "r");
	cp = curl_easy_init();
	if(cp){

		sprintf(str, "%s:%s", userid, password);
		curl_easy_setopt(cp, CURLOPT_USERPWD, str );
		/* use the read function */
		curl_easy_setopt(cp, CURLOPT_READFUNCTION, ftpRead);
		/* enable upload */
		curl_easy_setopt(cp, CURLOPT_UPLOAD, 1L);
		 /* now specify which file to upload */
		curl_easy_setopt(cp, CURLOPT_READDATA, hd_src);
		/* specify target */
		 sprintf(str, "sftp://%s/%s/%s", server, directory, ifile);
		curl_easy_setopt(cp, CURLOPT_URL, str);
		/* change file mode */
		curl_easy_setopt(cp, CURLOPT_NEW_FILE_PERMS, 0644L);
		 /* enable verbose for easier tracing */
		curl_easy_setopt(cp, CURLOPT_VERBOSE, 1L);


		crc = curl_easy_perform(cp);
		if(crc == 67){
			sprintf(str, "Authentication unsuccessful ** - %s", curl_easy_strerror(crc));
			bail(14, str);
		}
		if(crc == 48){
			sprintf(str, "fail to change permission  **- %s", curl_easy_strerror(crc));
			bail(99, str);
		}
		curl_easy_cleanup(cp);


	}else{
		sprintf(str, "Upload unsuccessful ** - %s", curl_easy_strerror(crc));
		bail(16, str);
	}
	Fclose(hd_src);

	curl_global_cleanup();
	return 0;
}
