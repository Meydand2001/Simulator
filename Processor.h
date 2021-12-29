#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "dictionary.h"

typedef struct {
	int Memory[4096];
} Memory;

typedef struct {
	Dictionary* executable;
	int Registers[16];
	int IO_Registers[23];
	int PC;
	int Flag;
	int isHandlingInterrupt; //1 if is in an interrupt routine, 0 if is not.
} Processor;
#endif
