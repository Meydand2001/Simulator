#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Number_operations.h"

// all number operation which are used are located here

/*******************
Function: hex2digit
Input: char hex
Operation: Converts char hex into int (unsigned)
*******************/
int hex2digit(char hex) {
	int dig = 0;
	if ('0' <= hex && hex <= '9') //case where hex is a digit.
		dig += hex - '0'; //
	else if ('A' <= hex && hex <= 'F') //case where hex is a letter.
		dig += hex - 'A' + 10;
	return dig;
}

/*******************
Function: hex2num
Input: char* hex
Operation: Converts char* hex into int (unsigned)
*******************/
int hex2num(char* hex) {
	int l = strlen(hex);
	int num = 0;
	_strrev(hex);  //reverse hex
	for (int i = 0; i < l; i++) //run backwards and sum over the digits with corresponding powers.
	{
		num += Pow(16, i) * hex2digit(hex[i]);
	}
	return num;
}

/*******************
Function: signedbin2num
Input: char* bin
Operation: Converts char* bin into int (signed)
*******************/
int signedbin2num(char* bin) { // checked get a hex string and returns an unsigned int.
	int l = strlen(bin);
	int num = 0;
	_strrev(bin);  //reverse bin
	for (int i = 0; i < l - 1; i++) //run backwards and sum over the digits with corresponding powers.
	{
		if (bin[i] == '1')
			num += Pow(2, i);
	}

	if (bin[l - 1] == '1') //if MSB=1 (LSB of reversed bin), deduct 2^(length of bin-1).
		num -= Pow(2, l - 1);

	return num;
}

/*******************
Function: signedhex2num
Input: char* hex, int length
Operation: Converts char* hex into int (signed, using 'length' bits)
*******************/
int signedhex2num(char* hex, int length) {
	char bin[48] = "";
	hex2bin(hex, bin, length); //bin=hex converted to bin
	int n = signedbin2num(bin); //n=bin converted to int (signed)
	return n;
}

/*******************
Function: complete
Input: char* bin, int l, int pos
Operation: Completes a binary string 'bin' to bit length l
with zeros or zeros and one at the start, depending on pos.
*******************/
void complete(char* bin, int l, int pos) {
	int n = l - strlen(bin);
	char ln[50] = "";
	if (pos == 1) { //complete with zeros
		for (int i = 0; i < n; i++) {
			strcat(ln, "0");
		}
		strcat(ln, bin);
		strcpy(bin, ln); //bin = ln;
	}
	else { //complete with one and then zeros,
		strcat(ln, "1");
		for (int i = 1; i < n; i++) {
			strcat(ln, "0");
		}
		strcat(ln, bin);
		strcpy(bin, ln); //bin = ln;
	}
}

/*******************
Function: pad0
Input: char* num, int l, int desiredLength
Operation: pads a string with zeros at the start,
to a length of 'desiredLength'
*******************/
void pad0(char* num, int desiredLength) {
	int l = strlen(num);
	int numOfZeros = desiredLength - l;
	num = _strrev(num); //reverse num
	for (int i = 0; i < numOfZeros; i++) { //add the zeros
		strcat(num, "0");
	}
	num = _strrev(num); //reverse num back
}

/*******************
Function: paddednum2hex
Input: int num, char* hex, int desiredLength
Operation: converts a number to hex (signed),
pads with zeros to 'desiredLength'.
*******************/
void paddednum2hex(int num, char* hex, int desiredLength) {
	_itoa(num, hex, 16);
	pad0(hex, desiredLength);
	int l = strlen(hex);
	for (int i = 0; i < l; i++) {
		hex[i] = toupper(hex[i]);
	}
}

/*******************
Function: hex2bin
Input: char num[], char* bin, int l
Operation: Converts num (in hex) into binary string bin, using l bits. 
*******************/
void hex2bin(char num[], char* bin, int l) { // make sure bin initialized to "".
	int i = 0;
	while (num[i] != '\0') {
		if (num[i] == '0') {
			strcat(bin, "0000");
		}
		else if (num[i] == '1') {
			strcat(bin, "0001");
		}
		else if (num[i] == '2') {
			strcat(bin, "0010");
		}
		else if (num[i] == '3') {
			strcat(bin, "0011");
		}
		else if (num[i] == '4') {
			strcat(bin, "0100");
		}
		else if (num[i] == '5') {
			strcat(bin, "0101");
		}
		else if (num[i] == '6') {
			strcat(bin, "0110");
		}
		else if (num[i] == '7') {
			strcat(bin, "0111");
		}
		else if (num[i] == '8') {
			strcat(bin, "1000");
		}
		else if (num[i] == '9') {
			strcat(bin, "1001");
		}
		else if (num[i] == 'A') {
			strcat(bin, "1010");
		}
		else if (num[i] == 'B') {
			strcat(bin, "1011");
		}
		else if (num[i] == 'C') {
			strcat(bin, "1100");
		}
		else if (num[i] == 'D') {
			strcat(bin, "1101");
		}
		else if (num[i] == 'E') {
			strcat(bin, "1110");
		}
		else if (num[i] == 'F') {
			strcat(bin, "1111");
		}
		i++;
	}
	complete(bin, l, 1); //completes bin to l bits.
}

/*******************
Function: signednum2bin
Input: int number, char* bin, int l
Operation: Converts number to binary string bin (signed).
*******************/
void signednum2bin(int number, char* bin, int l) {
	int pos = 1;
	if (number < 0) {
		number = number + Pow(2, l - 1);
		pos = 0;
	}
	int count = 0;
	int flag = 0;
	char st[13] = "";
	char a[2] = "";
	while (number != 0) {
		if (number % 2 != 0) {
			strcpy(a, "1");
		}
		else {
			strcpy(a, "0");
		}
		strcat(st, a);
		number /= 2;
	}

	_strrev(st); //reverse st
	strcat(bin, st); //ad st to bin
	complete(bin, l, pos); //complete to l bits.
}

/*******************
Function: Pow
Input: int base, int power
Operation: Returns base^power.
*******************/
int Pow(int base, int power) {
	int result = 1;
	for (int i = 0; i < power; i++) { //multiply base by its self 'power' times.
		result *= base;
	}
	return result;
}

/*******************
Function: bin2hex
Input: char bin[], char* hex, int l
Operation: convert binary string to hex string.
*******************/
void bin2hex(char bin[], char* hex, int l) {
	char temp[20] = "";
	char digit[5] = "0000";
	for (int i = 0; i < l; i++) {
		for (int j = 0; j < 4; j++) {
			digit[j] = bin[4 * i + j];
		}
		if (strcmp(digit, "0000") == 0) {
			strcat(temp, "0");
		}
		else if (strcmp(digit, "0001") == 0) {
			strcat(temp, "1");
		}
		else if (strcmp(digit, "0010") == 0) {
			strcat(temp, "2");
		}
		else if (strcmp(digit, "0011") == 0) {
			strcat(temp, "3");
		}
		else if (strcmp(digit, "0100") == 0) {
			strcat(temp, "4");
		}
		else if (strcmp(digit, "0101") == 0) {
			strcat(temp, "5");
		}
		else if (strcmp(digit, "0110") == 0) {
			strcat(temp, "6");
		}
		else if (strcmp(digit, "0111") == 0) {
			strcat(temp, "7");
		}
		else if (strcmp(digit, "1000") == 0) {
			strcat(temp, "8");
		}
		else if (strcmp(digit, "1001") == 0) {
			strcat(temp, "9");
		}
		else if (strcmp(digit, "1010") == 0) {
			strcat(temp, "A");
		}
		else if (strcmp(digit, "1011") == 0) {
			strcat(temp, "B");
		}
		else if (strcmp(digit, "1100") == 0) {
			strcat(temp, "C");
		}
		else if (strcmp(digit, "1101") == 0) {
			strcat(temp, "D");
		}
		else if (strcmp(digit, "1110") == 0) {
			strcat(temp, "E");
		}
		else if (strcmp(digit, "1111") == 0) {
			strcat(temp, "F");
		}
	}
	strcpy(hex, temp);
}

/*******************
Function: num2hex
Input: int number, char* hex, int l
Operation: convert number to hex string (signed).
*******************/
void num2hex(int number, char* hex, int l) {
	char bin[48] = "";
	signednum2bin(number, bin, l);
	bin2hex(bin, hex, l);
}
