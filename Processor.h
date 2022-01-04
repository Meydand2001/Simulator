#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "Number_operations.h"
#include "Dictionary.h"



//typedef struct {
//	int enable;
//	int current;
//	int max;
//}Timer;


typedef struct {
	int pixels[256 * 256]; //

}Monitor;


typedef struct {
	int disk[16384];
	int sector_size;
	int cyclesSinceStart;
}Harddisk;


typedef struct {
	int Memory[4096];
} Memory;


Memory* allocated_mem();
void destroy_mem(Memory* memory);
void init_memory(Memory* memory, Dictionary* dmemin);

typedef struct {
	Dictionary* executable;
	Dictionary* irq2in;
	int Registers[16];
	int IO_Registers[23];
	int PC;
	int Flag;  //1 if processor is running, 0 if halt command was issued.
	int isHandlingInterrupt; //1 if is in an interrupt routine, 0 if is not.
	int hasJumped; // 1 if there was a jump/branch 0 otherwise.
} Processor;

void timer_handler(Processor* SIMP);


Monitor* allocatemonitor();
void init_monitor(Monitor* m);
void write_pixel(Monitor* m, int address, int value);
void monitor_handler(Monitor* monitor, Processor* SIMP);
void get_pixel_content(Monitor* m, int content[]);


Harddisk* allocatedisk();
void init_hard_disk(Harddisk* hd, Dictionary* diskin);
//void get_content(Harddisk* hd, int content[]);
void read_sector(Memory* memory, Harddisk* hd, int sector, int buffer);
void write_sector(Memory* memory, Harddisk* hd, int sector, int buffer);
void hard_disk_handler(Memory* memory, Harddisk* hd, Processor* SIMP);


Processor* allocated_pro();
void destroy_pro(Processor* SIMP);
void init_Processor(Processor* SIMP, Dictionary* executable, Dictionary* irq2in);
void execute_code(Processor* SIMP, Memory* memory, Harddisk* hd, Monitor* monitor, Dictionary* trace,
	Dictionary* hwregtrace, Dictionary* leds, Dictionary* display);

#endif
