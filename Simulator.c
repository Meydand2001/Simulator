#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"
#include "hardware.h"
#include "Processor.h"
#include "NumberOperations.h"



void readfile(char* filename, Dictionary* data) {//works
	FILE* filepointer;
	filepointer = fopen(filename, "r");
	char line[50];
	int i = 0;
	while (!feof(filepointer)) {
		fgets(line, 500, filepointer);
		Element* row = allocate();
		init_element(row, i, line);
		add_element(data, row);
		i++;
	}
	fclose(filepointer);
}

void writefile(char* filename, Dictionary* data) {//works
	FILE* filepointer;
	filepointer = fopen(filename, "w");
	char line[50];
	Element* list = data->list;
	for (int i = 0; i < data->number_of_elements; i++) {
		strcpy(line, list[i].line);
		strcat(line, "\n");
		fprintf(filepointer, line);
	}
	fclose(filepointer);

}



//void dmemin(struct Memory* memory, char* filename) {
//	FILE* filepointer;
//	char line[20];
//	filepointer = fopen(filename, 'r');
//	for (int i = 0; i < 1024; i++) {
//
//	}
//}


void diskin() {

}

void irq2in() {

}

void dmemout_handler(Memory* memory,Dictionary* dmemout) {
	char line[50];
	for (int i = 0; i < 4096; i++) {
		Element* e = allocate();
		num2hex(memory->Memory[i], line, 8);
		// num to hex form memory->Memory[i] to line
		init_element(e, i, line);
		add_element(dmemout, e);
	}
}

void regout_handler(Processor* SIMP, Dictionary* regout) {
	char line[50];
	for (int i = 0; i < 13; i++) {
		Element* e = allocate();
		num2hex(SIMP->Registers[i + 3], line, 8);
		// num to hex form SIMP->registers[i+3] to line
		init_element(e, i, line);
		add_element(regout, e);
	}
}

void trace_handler(struct Processor* SIMP, char* filename) {

}

void hwregtrace_handler(struct Processor* SIMP, char* filename) {

}

void cycles_handler(Processor* SIMP, Dictionary* cycles) {
	char line[50];
	Element* e = allocate();
	num2hex(SIMP->IO_Registers[8], line, 8);
	// num to hex form SIMP->IOregisters[8] to line
	init_element(e, 1, line);
	add_element(dmemout_handler, e);
}

void leds_handler(struct Processor* SIMP, char* filename) {

}

void display_handler(struct Processor* SIMP) {

}

void monitorout_handler(Monitor* monitor, Dictionary* monitorout) {
	char line[50];
	for (int i = 0; i < 256*256; i++) {
		Element* e = allocate();
		num2hex(monitor->pixels[i], line, 2);
		// num to hex form monitor->pixel[i] to line.
		init_element(e, i, line);
		add_element(monitorout, e);
	}
}


void diskout_handler(Hard_disk* hard_disk, Dictionary* diskout) {
	char line[50];
	for (int i = 0; i < 16384; i++) {
		Element* e = allocate();
		num2hex(hard_disk->disk[i], line, 8);
		// num to hex form hard_disk->disk[i] to line.
		init_element(e, i, line);
		add_element(diskout, e);
	}
}


void main()
{
	Dictionary* imemin = allocatedict();
	init_dictionary(imemin, 4096);
	Dictionary* dmemin = allocatedict();
	init_dictionary(dmemin, 4096);
	Dictionary* diskin = allocatedict();
	init_dictionary(diskin, 16384);
	Dictionary* irq2in = allocatedict();
	init_dictionary(irq2in, 64);
	// here we extract the data from the files.
	readfile("Text1.txt", imemin);
	readfile("Text2.txt", dmemin);
	readfile("Text3.txt", diskin);
	readfile("Text4.txt", irq2in);

	// here we initialize the processor and the hardware.
	Processor* SIMP = allocated_pro();
	init_Processor(SIMP, imemin);
	Memory* memory = allocated_mem();
	init_memory(memory, dmemin);
	Hard_disk* hard_disk = allocatedisk();
	init_hard_disk(hard_disk, diskin);
	Monitor* monitor = allocatemonitor();
	init_monitor(monitor);
	destroydict(imemin);
	destroydict(dmemin);
	destroydict(diskin);
	destroydict(irq2in);

	Dictionary* trace = allocatedict();
	init_dictionary(trace, 16384);

	Dictionary* hwregtrace = allocatedict();
	init_dictionary(hwregtrace, 4096);

	Dictionary* leds = allocatedict();
	init_dictionary(leds, 128);

	Dictionary* display = allocatedict();
	init_dictionary(display, 128);
	//here we run the code.
	execute_code(SIMP, memory, hard_disk, monitor, trace, hwregtrace, leds, display);



	// extract data after the run to the files.
	Dictionary* dmemout = allocatedict();
	init_dictionary(dmemout, 4096);
	dmemout_handler(memory, dmemout);

	Dictionary* regout = allocatedict();
	init_dictionary(regout, 13);
	regout_handler(SIMP, regout);

	Dictionary* diskout = allocatedict();
	init_dictionary(diskout, 16384);
	diskout_handler(hard_disk, diskout);

	Dictionary* cycles = allocatedict();
	init_dictionary(cycles, 1);
	cycle_handler(SIMP, cycles);

	Dictionary* monitorout = allocatedict();
	init_dictionary(monitorout, 256*256);
	monitorout_handler(monitor, monitorout);



	writefile("Text5.txt", dmemout);
	writefile("Text6.txt", regout);



}
