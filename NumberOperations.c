
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Number_Operations.h"


// all number operation which are used are located here.


int hex2digit(char hex) {
	int dig = 0;
	if ('0' <= hex && hex <= '9')
		dig += hex - '0';
	else if ('A' <= hex && hex <= 'F')
		dig += hex - 'A' + 10;
	return dig;
}

int hex2num(char* hex) {
	int L = strlen(hex);
	int num = 0;
	char* reverse_hex = _strrev(hex);
	for (int i = 0; i < L; i++) //run backwards and sum over the digits with corresponding powers.
	{
		//printf("%d\n", num);
		num += Pow(16, i) * Hex2Digit(reverse_hex[i]);
	}
	return num;
}

void complete(char* bin, int l, int pos) {
	int n = l - strlen(bin);
	char ln[20] = "";
	if (pos == 1) {
		for (int i = 0; i < n; i++) {
			strcat(ln, "0");
		}
		strcat(ln, bin);
		strcpy(bin, ln);
	}
	else {
		strcat(ln, "1");
		for (int i = 1; i < n; i++) {
			strcat(ln, "0");
		}
		strcat(ln, bin);
		strcpy(bin, ln);
	}
}


void hex2bin(char num[], char* bin) {
	int i = 2;
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
	complete(bin, 12, 1);
}

void num2bin(int number, char* bin) {
	int pos = 1;
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

	_strrev(st);
	strcat(bin, st);
	complete(bin, 12, pos);

}

void signednum2bin(int number, char* bin) {//needs a slightly better solution.
	int pos = 1;
	if (number < 0) {
		number = number + 2048;
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

	_strrev(st);
	strcat(bin, st);
	complete(bin, 12, pos);
}




int Pow(int base, int power) {
	int result = 1;
	for (int i = 0; i < power; i++) {
		result *= base;
	}
	return result;
}


