#ifndef NUMBEROPERATIONS_H
#define NUMBEROPERATIONS_H


int hex2num(char* hex);
int hex2digit(char hex);
int Pow(int base, int power);
void complete(char* bin, int l, int pos);
void hex2bin(char num[], char* bin);
void num2bin(int number, char* bin);
void signednum2bin(int number, char* bin);
void dectobin(char num[], char* bin, int v);
void bin2hex(char bin[], char* hex, int v);
void num2hex(int number, char* hex, int v);

#endif
