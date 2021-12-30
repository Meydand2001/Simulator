#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hardware.h"

//timer functions 

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

void timer_handler(Processor* SIMP) {
	
	if (SIMP->IO_Registers[11] != 0) {
		if (SIMP->IO_Registers[12] >= SIMP->IO_Registers[13]) {
			SIMP->IO_Registers[3] = 1;
			SIMP->IO_Registers[12] = 0;// add irq
		}
		else {
			SIMP->IO_Registers[12]++;
		}
	}

}


// monitor functions

Monitor* allocatemonitor() {
	Monitor* m = (Monitor*)malloc(sizeof(Monitor));
	if (m == NULL) {
		return NULL;
	}
	return m;
}

void init_monitor(Monitor* m) {
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			m->pixels[i][j] = 0;
		}
	}
}

void write_pixel(Monitor* m, int address, int value) {
	m->command = 1;
	m->address = address;
	int i = address / 256;
	int j = address % 256;
	m->pixels[i][j] = value;
	m->command = 0;
}

void get_pixel_content(Monitor* m, int* content[]) {//for now.
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			content[i][j] = m->pixels[i][j];
		}
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

void init_Timer(Hard_disk* hd, int content[]) {
	for (int i = 0; i < 16384; i++) {
		hd->disk[i] = content[i];
	}
	hd->sector_size = 128;
	hd->cycle = 1024;
	hd->status = 0;
	hd->command = 0;
}

void get_content(Hard_disk* hd, int content[]) {
	for (int i = 0; i < 16384; i++) {
		content[i] = hd->disk[i];
	}
}

void read_sector(Hard_disk* hd, int sector, int buffer[]) {
	hd->status = 1;
	hd->command = 1;
	for (int i = 0; i < hd->sector_size; i++) {
		buffer[i] = hd->disk[hd->sector_size * sector + i];
	}
	hd->command = 0;
	hd->status = 0;
}

void write_sector(Hard_disk* hd, int sector, int buffer[]) {
	hd->status = 1;
	hd->command = 2;
	for (int i = 0; i < hd->sector_size; i++) {
		hd->disk[hd->sector_size * sector + i] = buffer[i];
	}
	hd->command = 0;
	hd->status = 0;
}
