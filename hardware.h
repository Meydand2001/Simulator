#ifndef HARDWARE_H
#define HARDWARE_H
#define Disk_Size 16384;

#include "Processor.h"

typedef struct {
	int enable;
	int current;
	int max;
}Timer;

Timer* allocatetimer();
void init_timer(Timer* t, int maxtime);
void run(Timer* t);
void timer_handler(Processor* SIMP);

typedef struct {
	int pixels[256*256]; //

}Monitor;

Monitor* allocatemonitor();
void init_monitor(Monitor* m);
void write_pixel(Monitor* m, int address, int value);
void monitor_handler(Monitor* monitor, Processor* SIMP);
void get_pixel_content(Monitor* m, int content[]);

typedef struct {
	int disk[16384];
	//Timer* t;
	int sector_size;
	int cyclesSinceStart;
	//int status;
	//int command;
}Hard_disk;



Hard_disk* allocatedisk();
void init_hard_disk(Hard_disk* hd, int content[]);
void get_content(Hard_disk* hd, int content[]);
void read_sector(Memory* memory, Hard_disk* hd, int sector, int buffer);
void write_sector(Memory* memory, Hard_disk* hd, int sector, int buffer);
void hard_disk_handler(Memory* memory,Hard_disk* hd, Processor* SIMP);

#endif 
