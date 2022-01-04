#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Dictionary.h"
//#include "Hardware.h"
#include "Processor.h"
#include "Number_operations.h"




void mainjh() {
	char bin[20]="";
	char hex[20] = "";
	int n = 37;
	num2hex(n, hex, 12);
	printf("%s\n", hex);
	
}


void readfile(char* filename, Dictionary* data) {//works
	FILE* filepointer;
	filepointer = fopen(filename, "r");
	char line[50];
	int i = 0;
	printf("%s\n", filename);
	while (!feof(filepointer)) {
		fgets(line, 50, filepointer);
		if (strlen(line) == 13);
		{
			line[12] = '\0';
		}
		Element* row = allocate();
		init_element(row, i, line);
		//printf("row: %d line: %s ", row->row_number, row->line);
		add_element(data, row);
		//printf("row: %d line: %s ", data->list[i].row_number, data->list[i].line);
		i++;
	}
	fclose(filepointer);
}



void writefile(char* filename, Dictionary* data) {//works
	FILE* filepointer;
	filepointer = fopen(filename, "w");
	char line[500];
	Element* list = data->list;
	for (int i = 0; i < data->number_of_elements; i++) {
		strcpy(line, list[i].line);
		strcat(line, "\n");
		fprintf(filepointer, line);
	}
	fclose(filepointer);
}

void writeyuvfile(char* filename, Dictionary* monitorout) {
	FILE* filepointer;
	filepointer = fopen(filename, "wb");
	char line[6]="";
	//char binline[20]="";
	Element* list = monitorout->list;
	for (int i = 0; i < monitorout->number_of_elements; i++) {
		strcpy(line, list[i].line);
		//printf("%d\n", i);
		//hex2bin(line, binline, 8);
		int pixel = hex2num(line);
		char pixelbyte = (char)(pixel);
		//strcat(binline, "\n");
		fwrite(&pixelbyte,1,1,filepointer);
		//fprintf(filepointer, binline);
		//strcpy(binline, "");
	}
	fclose(filepointer);
}

void dmemout_handler(Memory* memory, Dictionary* dmemout) {
	char line[50];
	for (int i = 0; i < 4096; i++) {
		Element* e = allocate();
		//num2hex(memory->Memory[i], line, 8);
		// num to hex form memory->Memory[i] to line
		paddednum2hex(memory->Memory[i], line, 8);
		init_element(e, i, line);
		add_element(dmemout, e);
	}
}

void regout_handler(Processor* SIMP, Dictionary* regout) {
	char line[50];
	for (int i = 0; i < 13; i++) {
		Element* e = allocate();
		//num2hex(SIMP->Registers[i + 3], line, 8);
		//num to hex form SIMP->registers[i+3] to line
		paddednum2hex(SIMP->Registers[i+3], line, 8);
		init_element(e, i, line);
		add_element(regout, e);
	}
}

void cycles_handler(Processor* SIMP, Dictionary* cycles) {
	char line[50];
	Element* e = allocate();
	//num2hex(SIMP->IO_Registers[8], line, 8);
	// num to hex form SIMP->IOregisters[8] to line
	//paddednum2hex(SIMP->IO_Registers[8], line, 8);
	sprintf(line, "%u", SIMP->IO_Registers[8]);
	init_element(e, 1, line);
	add_element(cycles, e);
}


void monitorout_handler(Monitor* monitor, Dictionary* monitorout) {
	char line[10];
	for (int i = 0; i < 256 * 256; i++) {
		Element* e = allocate();
		//num2hex(monitor->pixels[i], line, 2);
		// num to hex form monitor->pixel[i] to line.
		paddednum2hex(monitor->pixels[i], line, 2);
		init_element(e, i, line);
		add_element(monitorout, e);
	}
}


void diskout_handler(Harddisk* hard_disk, Dictionary* diskout) {
	char line[50];
	for (int i = 0; i < 16384; i++) {
		Element* e = allocate();
		//num2hex(hard_disk->disk[i], line, 8);
		// num to hex form hard_disk->disk[i] to line.
		paddednum2hex(hard_disk->disk[i], line, 8);
		init_element(e, i, line);
		add_element(diskout, e);
	}
}

//void fillimemin(Dictionary* imemin) {// maybe add \n to the last.
//	char line[50] = "000000000000\n";
//	for (int i = imemin->number_of_elements; i < imemin->max; i++) {
//		Element* row = allocate();
//		init_element(row, i, line);
//		add_element(imemin, row);
//		i++;
//	}
//}


void mainlo() {
	char hex[30] = "0FFD";
	int n = signedhex2num(hex, 16);
	printf("%d", n);
	

}

void main() {
	Dictionary* imemin = allocatedict();
	init_dictionary(imemin, 4096);
	Dictionary* dmemin = allocatedict();
	init_dictionary(dmemin, 4096);
	Dictionary* diskin = allocatedict();
	init_dictionary(diskin, 16384);
	Dictionary* irq2in = allocatedict();
	init_dictionary(irq2in, 64);
	// here we extract the data from the files.
	readfile("imemin.txt", imemin);
	//fillimemin(imemin);
	readfile("dmemin.txt", dmemin);
	readfile("diskin.txt", diskin);
	readfile("irq2in.txt", irq2in);

	Processor* SIMP = allocated_pro();
	init_Processor(SIMP, imemin, irq2in);// the dictionaries are in.
	Memory* memory = allocated_mem();
	init_memory(memory, dmemin);
	//printf("%d\n", memory->Memory[63]);
	Harddisk* hard_disk = allocatedisk();
	init_hard_disk(hard_disk, diskin);
	Monitor* monitor = allocatemonitor();
	init_monitor(monitor);

	Dictionary* trace = allocatedict();
	init_dictionary(trace, 16384);

	Dictionary* hwregtrace = allocatedict();
	init_dictionary(hwregtrace, 4096);

	Dictionary* leds = allocatedict();
	init_dictionary(leds, 128);

	Dictionary* display = allocatedict();
	init_dictionary(display, 128);

	// main battle point.
	execute_code(SIMP, memory, hard_disk, monitor, trace, hwregtrace, leds, display);

	// extract data after the run to the files.
	Dictionary* dmemout = allocatedict();
	init_dictionary(dmemout, 4096);
	dmemout_handler(memory, dmemout);
	writefile("dmemout.txt", dmemout);

	Dictionary* regout = allocatedict();
	init_dictionary(regout, 13);
	regout_handler(SIMP, regout);
	writefile("regout.txt", regout);

	Dictionary* cycles = allocatedict();
	init_dictionary(cycles, 1);
	cycles_handler(SIMP, cycles);
	writefile("cycles.txt", cycles);

	Dictionary* monitorout = allocatedict();
	init_dictionary(monitorout, 256 * 256);
	monitorout_handler(monitor, monitorout);
	writefile("monitor.txt", monitorout);
	writeyuvfile("monitor.yuv", monitorout);

	Dictionary* diskout = allocatedict();
	init_dictionary(diskout, 16384);
	diskout_handler(hard_disk, diskout);
	writefile("diskout.txt", diskout);

	
	writefile("trace.txt", trace);
	writefile("hwregtrace.txt", hwregtrace);
	writefile("leds.txt", leds);
	writefile("display7seg.txt", display);



}
