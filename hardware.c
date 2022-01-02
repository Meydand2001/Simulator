#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hardware.h"

//timer functions 
/*
Timer* allocatetimer() {
	Timer* t = (Timer*)malloc(sizeof(Timer));
	if (t == NULL) {
		return NULL;
	}
	return t;
}
void init_Timer(Timer* t, int maxtime) {
	t->enable = 0;
	t->current = 0;
	t->max = maxtime;
}
void run(Timer* t) {
	if (t->enable != 0) {
		if (t->current == t->max) {
			t->current = 0;// add irq
		}
		else {
			t->current++;
		}
	}
}
*/
void timer_handler(Processor* SIMP) {

	if (SIMP->IO_Registers[11] != 0) {
		if (SIMP->IO_Registers[12] >= SIMP->IO_Registers[13]) { //timercurrent larger than (or equal) to timer max.
			SIMP->IO_Registers[3] = 1;  //irqstatus=1.
			SIMP->IO_Registers[12] = 0; //timercurrent=0.
		}
		else {
			SIMP->IO_Registers[12]++; //timercurrent++.
		}
	}

}

// monitor functions

Monitor* allocatemonitor() {
	Monitor* monitor = (Monitor*)malloc(sizeof(Monitor));
	if (monitor == NULL) {
		return NULL;
	}
	return monitor;
}

void init_monitor(Monitor* monitor) {
	for (int i = 0; i < 256 * 256; i++) {
		monitor->pixels[i] = 0;
	}
}

void write_pixel(Monitor* monitor, int address, int value) {
	monitor->pixels[address] = value;
}

void get_pixel_content(Monitor* monitor, int content[]) {//for now.
	for (int i = 0; i < 256 * 256; i++) {
		content[i] = monitor->pixels[i];
	}
}

void monitor_handler(Monitor* monitor, Processor* SIMP)
{
	if (SIMP->IO_Registers[22] == 1)
	{
		write_pixel(monitor, SIMP->IO_Registers[20], SIMP->IO_Registers[21]);
		SIMP->IO_Registers[22] = 0;
	}
}

// hard disk functions

Hard_disk* allocatedisk() {
	Hard_disk* hd = (Hard_disk*)malloc(sizeof(Hard_disk));
	if (hd == NULL) {
		return NULL;
	}
	return hd;
}

void init_hard_disk(Hard_disk* hd, Dictionary* diskin) {
	for (int i = 0; i < 16384; i++) {
		if (i < diskin->number_of_elements) {
			//hd->disk[i]=translate(dmemin->list[i])
		}
		else {
			hd->disk[i] = 0;
		}
	}
	hd->sector_size = 128;
}

void get_content(Hard_disk* hd, int content[]) {
	for (int i = 0; i < 16384; i++) {
		content[i] = hd->disk[i];
	}
}

void read_sector(Memory* memory, Hard_disk* hd, int sector, int buffer) {
	for (int i = 0; i < hd->sector_size; i++) {
		memory->Memory[buffer + i] = hd->disk[hd->sector_size * sector + i];
	}
}

void write_sector(Memory* memory, Hard_disk* hd, int sector, int buffer) {
	for (int i = 0; i < hd->sector_size; i++) {
		hd->disk[hd->sector_size * sector + i] = memory->Memory[buffer + i];
	}
}

void hard_disk_handler(Memory* memory, Hard_disk* hd, Processor* SIMP)
{
	int cmd = SIMP->IO_Registers[14];
	int disk_status = SIMP->IO_Registers[17];
	if (disk_status == 0) {
		switch (cmd)
		{
		case 1:
		{
			read_sector(memory, hd, SIMP->IO_Registers[15], SIMP->IO_Registers[16]);
			SIMP->IO_Registers[17] = 1;
			hd->cyclesSinceStart = 1;
		} break;
		case 2:
		{
			write_sector(memory, hd, SIMP->IO_Registers[15], SIMP->IO_Registers[16]);
			SIMP->IO_Registers[17] = 1;
			hd->cyclesSinceStart = 1;
		} break;

		default:
			break;
		}

	}
	else
	{
		if (hd->cyclesSinceStart >= 1024) {
			SIMP->IO_Registers[17] = 0;
			SIMP->IO_Registers[14] = 0;
			SIMP->IO_Registers[4] = 1;
		}
		else
			hd->cyclesSinceStart++;
	}

}
