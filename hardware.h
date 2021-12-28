#ifndef HARDWARE_H
#define HARDWARE_H

typedef struct {
	int enable;
	int current;
	int max;
}Timer;

Timer* allocatetimer();
void init_hard_disk(Timer* t, int maxtime);
void run(Timer* t);

typedef struct {
	int pixels[256][256];
	int address;
	int command;


}Monitor;

Monitor* allocatemonitor();
void init_monitor(Monitor* m);
void write_pixel(Monitor* m, int address, int value);
void get_pixel_content(Monitor* m, int* content[]);

typedef struct {
	int disk[16384];
	//Timer* t;
	int sector_size;
	int cycle;
	int status;
	int command;
}Hard_disk;



Hard_disk* allocatedisk();
void init_hard_disk(Hard_disk* hd, int content[]);
void get_content(Hard_disk* hd, int content[]);
void read_sector(Hard_disk* hd, int sector, int buffer[]);
void write_sector(Hard_disk* hd, int sector, int buffer[]);
#endif 

