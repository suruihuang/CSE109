/*
 * Course: CSE109
 * Semester and Year: Fall 2019
 * Assignment: Count
 * File Purpose: prog4_funcs.cpp
 * Author: Huang, Surui
 * User ID: suh222
 */

#include <iostream>
#include <stdlib.h>     /* exit, EXIT_FAILURE */
#include <string>
#include "prog4_funcs.h"

/*
 * if no argument is entered, prompt the user to enter
 * if file is passed, read through the file
 * else help
 * */
void parseArgs(int argc, char *argv[]){
	switch(argc){
		case 1:
			break;
		default:
			//these are help messages
			std::cout << "This program counts the occurrences of each type of byte streaming from STDIN." <<std::endl;
			std::cout <<"Specifically, it will count occurrences of each letter (regardless of case)," <<std::endl;
			std::cout <<"each digit, each whitespace, all other printable characters (except extended-ASCII),"<<std::endl;
			std::cout <<"all extended-ASCII characters, and all newline characters.\n"<<std::endl;
			std::cout << "Usage: prog4 < someFile\n\t or\n\tprog4\n(Note: type input followed by Cntl+D)"<<std::endl;
			exit(0);
			break;
	}
}

void parseFile(Count *count){
	std::string str;
	unsigned char tmp;
	while(getline(std::cin, str)){
		for(unsigned int i = 0; i < str.size(); i++){
			//make tmp at str[i]
			tmp = str[i];
			switch(tmp){
			case 'a'...'z':					//lower case
				count->increment('l', tmp);
				break;
			case 'A'...'Z':					//upper case
				count->increment('L', tmp);
				break;
			case '0'...'9':					//digit
				count->increment('d', tmp);
				break;
			case 32:						//whitespace
				count->increment('w');
				break;
			case 128 ... 255:
				count->increment('E');
				break;
			default:
				count->increment('O');
				break;
			}
		}
		//add a new line at end of getline
		count->increment('N');
	}
	if(!std::cin.fail()){
		bail(10, "Reading from stdin failed");
	}
}

void bail(int code, const char *errmessage){
	 std::cerr << code << " - " << errmessage << std::endl;
}
