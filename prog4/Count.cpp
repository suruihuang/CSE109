/*
 * Course: CSE109
 * Semester and Year: Fall 2019
 * Assignment: Count
 * File Purpose: Count.cpp
 * Author: Huang, Surui
 * User ID: suh222
 */
#include "Count.h"

/* constructor */
Count::Count() {
	std::cout << "Count is being created" << std::endl;
	numWhiteSpace = 0;
	numOther = 0; // default
	numExtended = 0; //128 - 255
	numNewline = 0;
	numLetter = std::vector<unsigned int> (26,0);
	numDigit  = std::vector<unsigned int> (10,0);
}

void Count::increment(char nonLD){
	switch(nonLD){
	case 'w':					//whitespace
		numWhiteSpace++;
		break;
	case 'E':					//extended
		numExtended++;
		break;
	case 'N':					//newline
		numNewline++;
		break;
	default:
		numOther++;
	}
}

void Count::increment(char LD, unsigned char ascCode){

	switch(LD){
		case 'l':
			numLetter[ascCode - 'a']++;
			break;
		case 'L':
			numLetter[ascCode - 'A']++;
			break;
		case 'd':
			numDigit[ascCode - '0']++;
			break;
	}

}
/* getter: return total of count */
unsigned int Count::getTotal(){
	unsigned int totalCount = numWhiteSpace + numOther + numExtended + numNewline;
	for (std::vector<unsigned int>::iterator it = numLetter.begin(); it != numLetter.end(); ++it){
		totalCount += *it;
	}
	for (std::vector<unsigned int>::iterator it = numDigit.begin(); it != numDigit.end(); ++it){
		totalCount += *it;
	}

	return totalCount;
}

std::ostream & operator <<(std::ostream &os, Count &ct){
	int index_letter = 0;
	int index_digit = 0;
	//alphabet
	 std::string str = ("abcdefghijklmnopqrstuvwxyz");
	 std::string digit = ("0123456789");
	//print out numletter
	 for (std::string::iterator itr = str.begin(); itr!=str.end(); ++itr){
		os << "Number of letter " << *itr << ": "<< ct.numLetter.at(index_letter) <<std::endl;
		index_letter++;
	 }
	 for (std::string::iterator itr = digit.begin(); itr!=digit.end(); ++itr){
	 	os << "Number of digit " << *itr << ": " << ct.numDigit.at(index_digit) <<std::endl;
	 	index_digit++;
	 }
	os << "Number of Whitespace: " << ct.numWhiteSpace << std::endl;
	os << "Number of Other: " << ct.numOther << std::endl;
	os << "Number of Extended: " << ct.numExtended << std::endl;
	os << "Number of Newline: " << ct.numNewline << std::endl;
	os << std::endl;
	os << "Total number of characters: " << (ct.getTotal()) << std::endl;
	return os;
}

/* destructor */
Count::~Count(){
	std::cout << *this << std::endl;
	std::cout << "Count is being deleted " << std::endl;
}
