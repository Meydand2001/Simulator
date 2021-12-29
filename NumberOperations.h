#ifndef NUMBEROPERATIONS_H
#define NUMBEROPERATIONS_H


int hex2num(char* hex);
int hex2digit(char hex);
int Pow(int base, int power);
void complete(char* bin, int l, int pos);
void hex2bin(char num[], char* bin);
void num2bin(int number, char* bin);
void signednum2bin(int number, char* bin);

#endif