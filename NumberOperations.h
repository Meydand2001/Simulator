#ifndef NUMBEROPERATIONS_H
#define NUMBEROPERATIONS_H


int hex2num(char* hex);
int hex2digit(char hex);
int Pow(int base, int power);
void complete(char* bin, int l, int pos);
void hex2bin(char num[], char* bin, int l);
void num2bin(int number, char* bin);
void pad0(char* num, int desiredLength);
void paddednum2hex(int num, char* hex, int desiredLength);
void signednum2bin(int number, char* bin,int l);
void signeddec2bin(char num[], char* bin, int l);
void bin2hex(char bin[], char* hex, int l);
void num2hex(int number, char* hex, int v);

#endif
