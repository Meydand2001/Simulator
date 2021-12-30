#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"
#include "hardware.h"
#include "Processor.h"
#include "NumberOperations.h"

void imemin(Processor* SIMP, char* filename) {
	FILE* filepointer;
	char line[20];
	filepointer = fopen(filename, 'r');
	for (int i = 0; i < 1024; i++) {

	}
}

void dmemin(Memory* memory, char* filename) {
	FILE* filepointer;
	char line[20];
	filepointer = fopen(filename, 'r');
	for (int i = 0; i < 1024; i++) {

	}
}


void diskin() {

}

void irq2in() {

}

void dmemout(Memory* memory, char* filename) {
	FILE* filepointer;
	char line[20];
	filepointer = fopen(filename, 'w');
	for (int i = 0; i < 1024; i++) {
		// translate.
		fprintf(filepointer, line);
	}
}

void regout(Processor* SIMP, char* filename) {
	FILE* filepointer;
	char line[20];
	filepointer = fopen(filename, 'w');
	for (int i = 3; i < 16; i++) {
		// translate.
		fprintf(filepointer, line);
	}
}

void trace(Processor* SIMP, char* filename) {

}

void hwregtrace(Processor* SIMP, char* filename) {

}

void cycles(Processor* SIMP, char* filename) {

}

void leds(Processor* SIMP, char* filename) {

}

void display(Processor* SIMP) {

}

void monitorout() {

}


void diskout() {

}


void main()
{
	char a[] = "F4367C";
	//char* bla = _strrev(a);
	//printf("%s\n", bla);
	printf("%d\n", Hex2Num(a));
	//printf("%d", Pow(16,2));


}
