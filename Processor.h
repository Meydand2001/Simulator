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
	int pixels[256 * 256]; //256*256(=65536) array of pixel
} Monitor;


typedef struct {
	int disk[16384]; // 128 sectors * 128 words per sector(512 bytes)
	int sector_size; //sector_size=128.
	int cyclesSinceStart; //counts cycles since starting read/write operation,
						  //resets to 1 when starting a new read/write operation.
} HardDisk;


typedef struct {
	int Memory[4096];
} Memory;


typedef struct {
	Dictionary* executable;
	Dictionary* irq2in;
	int Registers[16];    //array of all regular (non-IO) Registers
	int IO_Registers[23]; //array of all IO Registers
	int PC;
	int Flag;  //1 if processor is running, 0 if halt instruction was issued.
	int isHandlingInterrupt; //1 if is in an interrupt routine, 0 if is not.
	int hasJumped; //1 if there was a jump/branch,0 otherwise.
} Processor;

/********Timer Functions*********/
void timer_handler(Processor* SIMP);


/********Monitor Functions*********/
Monitor* allocatemonitor();
void init_monitor(Monitor* m);
void write_pixel(Monitor* m, int address, int value);
void monitor_handler(Monitor* monitor, Processor* SIMP);


/********HardDisk Functions*********/
HardDisk* allocatedisk();
void init_hard_disk(HardDisk* hd, Dictionary* diskin);
void read_sector(Memory* memory, HardDisk* hd, int sector, int buffer);
void write_sector(Memory* memory, HardDisk* hd, int sector, int buffer);
void hard_disk_handler(Memory* memory, HardDisk* hd, Processor* SIMP);

/********Memory Functions*********/
Memory* allocate_mem();
void destroy_mem(Memory* memory);
void init_memory(Memory* memory, Dictionary* dmemin);

/********Processor Functions*********/
Processor* allocate_pro();
void destroy_pro(Processor* SIMP);
void init_Processor(Processor* SIMP, Dictionary* executable, Dictionary* irq2in);
void execute_code(Processor* SIMP, Memory* memory, HardDisk* hd, Monitor* monitor, Dictionary* trace,
	Dictionary* hwregtrace, Dictionary* leds, Dictionary* display);

#endif
