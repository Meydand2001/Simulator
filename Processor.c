#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Processor.h"
#include "Dictionary.h"
#include "Number_operations.h"


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

Harddisk* allocatedisk() {
	Harddisk* hd = (Harddisk*)malloc(sizeof(Harddisk));
	if (hd == NULL) {
		return NULL;
	}
	return hd;
}

void init_hard_disk(Harddisk* hd, Dictionary* diskin) {
	for (int i = 0; i < 16384; i++) {
		if (i < diskin->number_of_elements) {
			//hd->disk[i]=translate(dmemin->list[i])
			hd->disk[i] = signedhex2num(diskin->list[i].line, 32);
		}
		else {
			hd->disk[i] = 0;
		}
	}
	hd->sector_size = 128;
}

//void get_content(Harddisk* hd, int content[]) {
//	for (int i = 0; i < 16384; i++) {
//		content[i] = hd->disk[i];
//	}
//}

void read_sector(Memory* memory, Harddisk* hd, int sector, int buffer) {
	for (int i = 0; i < hd->sector_size; i++) {
		memory->Memory[buffer + i] = hd->disk[hd->sector_size * sector + i];
	}
}

void write_sector(Memory* memory, Harddisk* hd, int sector, int buffer) {
	for (int i = 0; i < hd->sector_size; i++) {
		hd->disk[hd->sector_size * sector + i] = memory->Memory[buffer + i];
	}
}

void hard_disk_handler(Memory* memory, Harddisk* hd, Processor* SIMP)
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




Memory* allocated_mem() {
	Memory* memory = (Memory*)malloc(sizeof(Memory));
	if (memory == NULL) {
		return NULL;
	}
	return memory;
}

void destroy_mem(Memory* memory) {
	free(memory);
}

void init_memory(Memory* memory, Dictionary* dmemin) {
	for (int i = 0; i < 4096; i++) {
		if (i < dmemin->number_of_elements) {
			memory->Memory[i] = signedhex2num(dmemin->list[i].line,32);
		}
		else {
			memory->Memory[i] = 0;
		}
	}
}


Processor* allocated_pro() {
	Processor* SIMP = (Processor*)malloc(sizeof(Processor));
	if (SIMP == NULL) {
		return NULL;
	}
	return SIMP;
}

void destroy_pro(Processor* SIMP) {
	destroydict(SIMP->executable);
	free(SIMP);
}

void init_Processor(Processor* SIMP, Dictionary* executable, Dictionary* irq2in) {
	SIMP->executable = executable;
	SIMP->irq2in = irq2in;
	for (int i = 0; i < 16; i++) {
		SIMP->Registers[i] = 0;
	}
	for (int i = 0; i < 23; i++) {
		SIMP->IO_Registers[i] = 0;
	}
	SIMP->IO_Registers[8] = 0;
	SIMP->PC = 0;
	SIMP->Flag = 1;
	SIMP->isHandlingInterrupt = 0;
	SIMP->hasJumped = 0;
}

void add(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	SIMP->Registers[rd_index] = SIMP->Registers[rs_index] + SIMP->Registers[rt_index] + SIMP->Registers[rm_index];
}

void sub(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	SIMP->Registers[rd_index] = SIMP->Registers[rs_index] - SIMP->Registers[rt_index] - SIMP->Registers[rm_index];
}

void mac(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	SIMP->Registers[rd_index] = SIMP->Registers[rs_index] * SIMP->Registers[rt_index] + SIMP->Registers[rm_index];
}

void and(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	SIMP->Registers[rd_index] = (SIMP->Registers[rs_index] & SIMP->Registers[rt_index]) & SIMP->Registers[rm_index];
}

void or (int rd_index, int rs_index, int rt_index, int rm_index, Processor * SIMP)
{
	SIMP->Registers[rd_index] = (SIMP->Registers[rs_index] | SIMP->Registers[rt_index]) | SIMP->Registers[rm_index];
}

void xor(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	SIMP->Registers[rd_index] = (SIMP->Registers[rs_index] ^ SIMP->Registers[rt_index]) ^ SIMP->Registers[rm_index];
}

void sll(int rd_index, int rs_index, int rt_index, int rm_index, Processor*SIMP)
{
	SIMP->Registers[rd_index] = SIMP->Registers[rs_index] << SIMP->Registers[rt_index];
}

void sra(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	SIMP->Registers[rd_index] = ~((~SIMP->Registers[rs_index]) >> SIMP->Registers[rt_index]);
}

void srl(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	SIMP->Registers[rd_index] = SIMP->Registers[rs_index] >> SIMP->Registers[rt_index];
}

void beq(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	if (SIMP->Registers[rs_index] == SIMP->Registers[rt_index]) {
		SIMP->PC = SIMP->Registers[rm_index] % Pow(2, 12);//mod 2^12 to take the low [11:0] bits.
		SIMP->hasJumped = 1;
	}
}

void bne(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	if (SIMP->Registers[rs_index] != SIMP->Registers[rt_index]) {
		SIMP->PC = SIMP->Registers[rm_index] % Pow(2, 12);//mod 2^12 to take the low [11:0] bits.
		SIMP->hasJumped = 1;
	}
}


void blt(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	if (SIMP->Registers[rs_index] < SIMP->Registers[rt_index]) {
		SIMP->PC = SIMP->Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
		SIMP->hasJumped = 1;
	}
}


void bgt(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	if (SIMP->Registers[rs_index] > SIMP->Registers[rt_index]) {
		SIMP->PC = SIMP->Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
		SIMP->hasJumped = 1;
	}
}


void ble(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	if (SIMP->Registers[rs_index] <= SIMP->Registers[rt_index]) {
		SIMP->PC = SIMP->Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
		SIMP->hasJumped = 1;
	}
}


void bge(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	if (SIMP->Registers[rs_index] >= SIMP->Registers[rt_index]) {
		SIMP->PC = SIMP->Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
		SIMP->hasJumped = 1;
	}
}

void jal(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	SIMP->Registers[rd_index] = SIMP->PC + 1;
	SIMP->PC = SIMP->Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
	SIMP->hasJumped = 1;
}

void lw(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP, Memory* memory)
{
	/*printf("\nLW\n");
	printf("%d\n",memory->Memory[2030]);

	printf(" index reg: %d, index mem: %d\n ", rd_index, SIMP->Registers[rs_index] + SIMP->Registers[rt_index]);
	printf(" old value: %d, new value: %d\n ", SIMP->Registers[rd_index], memory->Memory[SIMP->Registers[rs_index] + SIMP->Registers[rt_index]] );*/
	SIMP->Registers[rd_index] = memory->Memory[SIMP->Registers[rs_index] + SIMP->Registers[rt_index]] + SIMP->Registers[rm_index];
}

void sw(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP, Memory* memory)
{
	/*printf("\nSW\n");
	for (int i = 2047; i > 2029; i--) {
		printf("%d\n", memory->Memory[i]);
	}*/
	//memory->Memory[2040] = 20;
	//printf(" index mem: %d\n ", SIMP->Registers[rs_index] + SIMP->Registers[rt_index]);
	//printf(" old memory value: %d, new memory value : %d\n ", SIMP->Registers[rd_index], memory->Memory[SIMP->Registers[rs_index] + SIMP->Registers[rt_index]] + SIMP->Registers[rm_index]);
	int n = SIMP->Registers[rs_index] + SIMP->Registers[rt_index];
	int d = SIMP->Registers[rm_index] + SIMP->Registers[rd_index];
	memory->Memory[n] = d;
}

void reti(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	SIMP->PC = SIMP->IO_Registers[7];
	SIMP->isHandlingInterrupt = 0; //no longer handling interrupt
	SIMP->hasJumped = 1;

}

void in(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	SIMP->Registers[rd_index] = SIMP->IO_Registers[SIMP->Registers[rs_index] + SIMP->Registers[rt_index]];
}

void out(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	SIMP->IO_Registers[SIMP->Registers[rs_index] + SIMP->Registers[rt_index]] = SIMP->Registers[rm_index];
}

void halt(Processor* SIMP)
{
	SIMP->Flag = 0;
}

void insert_imm(Processor* SIMP, int imm1, int imm2) {
	SIMP->Registers[1] = imm1;
	SIMP->Registers[2] = imm2;
}


int get_opcode(char* row) {
	char op[20];
	//printf("%s", row);
	strncpy(op, row, 2);
	op[2] = '\0';
	//printf("%s",op);
	int opcode = hex2num(op);
	return opcode;
}

void get_indices(char* row, int indices[]) {
	char rd_hex, rs_hex, rt_hex, rm_hex;
	rd_hex = row[2];
	rs_hex = row[3];
	rt_hex = row[4];
	rm_hex = row[5];
	indices[0] = hex2digit(rd_hex);
	indices[1] = hex2digit(rs_hex);
	indices[2] = hex2digit(rt_hex);
	indices[3] = hex2digit(rm_hex);
}

void get_imm(char* row, int imm[]) {
	char imm1_hex[4];
	imm1_hex[3] = '\0';
	char imm2_hex[4];
	imm2_hex[3] = '\0';
	for (int i = 0; i < 3; i++)
	{
		imm1_hex[i] = row[i + 6];
		imm2_hex[i] = row[i + 9];
	}
	imm[0] = signedhex2num(imm1_hex,12);
	imm[1] = signedhex2num(imm2_hex,12);
	//imm[0]= atoi()
}


void index2name(int index, char* name) {
	switch (index) {
	case 0:
		strcpy(name, "irq0enable");
		break;
	case 1:
		strcpy(name, "irq1enable");
		break;
	case 2:
		strcpy(name, "irq2enable");
		break;
	case 3:
		strcpy(name, "irq0status");
		break;
	case 4:
		strcpy(name, "irq1status");
		break;
	case 5:
		strcpy(name, "irq2status");
		break;
	case 6:
		strcpy(name, "irqhandler");
		break;
	case 7:
		strcpy(name, "irqreturn");
		break;
	case 8:
		strcpy(name, "clks");
		break;
	case 9:
		strcpy(name, "leds");
		break;
	case 10:
		strcpy(name, "display7seg");
		break;
	case 11:
		strcpy(name, "timerenable");
		break;
	case 12:
		strcpy(name, "timercurrent");
		break;
	case 13:
		strcpy(name, "timermax");
		break;
	case 14:
		strcpy(name, "diskcmd");
		break;
	case 15:
		strcpy(name, "disksector");
		break;
	case 16:
		strcpy(name, "diskbuffer");
		break;
	case 17:
		strcpy(name, "diskstatus");
		break;
	case 18:
		strcpy(name, "reserved1");
		break;
	case 19:
		strcpy(name, "reserved2");
		break;
	case 20:
		strcpy(name, "monitoraddr");
		break;
	case 21:
		strcpy(name, "monitordata");
		break;
	case 22:
		strcpy(name, "monitorcmd");
		break;
	default:
		strcpy(name, "error");
		break;
	}

}

void write_trace(Processor* SIMP, Dictionary* trace, char* row) {
	printf("trace: ");
	char trace_line[500] = "";
	char hex[15];
	//num2hex(SIMP->PC, hex, 12);
	paddednum2hex(SIMP->PC,hex, 3);
	strcat(trace_line, hex);
	//printf("%s\n", trace_line);
	strcat(trace_line, " ");
	strcat(trace_line, row);
	//printf("%s\n", trace_line);
	for (int i = 0; i < 16; i++) {
		//num2hex(SIMP->Registers[i], hex, 32);
		paddednum2hex(SIMP->Registers[i], hex, 8);
		if (i == 2) {
			printf("imm2:  %d\n ", SIMP->Registers[i]);
		}
		//printf("%s",hex);
		strcat(trace_line, " ");
		strcat(trace_line, hex);
		//printf("%s\n", trace_line);
	}
	printf("%s\n", trace_line);
	Element* e = allocate();
	init_element(e, SIMP->PC, trace_line);
	add_element(trace, e);
}

void write_hwregtrace(Processor* SIMP, Dictionary* hwregtrace,int index1, int index2, int type) {
	printf("hwregtrace: ");
	char hwregtrace_line[200] = "";
	char piece[12];
	//long un = (unsigned long)(SIMP->IO_Registers[8]);//////// houston we have a problem.
	//printf("%u", un);

	/*unsigned int x = (unsigned int)(SIMP->IO_Registers[8]);
	printf("%u", x);*/
	sprintf(piece,"%u", SIMP->IO_Registers[8]);
	//_itoa((unsigned int)(SIMP->IO_Registers[8]), piece, 10);
	//printf("%s", piece);
	//paddednum2hex();
	strcat(hwregtrace_line, piece);
	if (type == 1) {
		strcat(hwregtrace_line, " WRITE ");
	}
	else {
		strcat(hwregtrace_line, " READ ");
	}
	//printf("%s\n", hwregtrace_line);
	// index to name function.
	index2name(SIMP->Registers[index1] + SIMP->Registers[index2], piece);
	strcat(hwregtrace_line, piece);
	strcat(hwregtrace_line, " ");
	char data[15];
	//printf("%d\n",SIMP->IO_Registers[SIMP->Registers[index1] + SIMP->Registers[index2]]);
	paddednum2hex(SIMP->IO_Registers[SIMP->Registers[index1] + SIMP->Registers[index2]], data, 8);
	strcat(hwregtrace_line, data);
	printf("%s\n", hwregtrace_line);
	Element* e = allocate();
	init_element(e, SIMP->PC, hwregtrace_line);
	add_element(hwregtrace, e);
}

void write_leds(Processor* SIMP, Dictionary* leds) {
	printf("leds: ");
	char leds_line[100] = "";
	char piece[12];
	_itoa((unsigned int)(SIMP->IO_Registers[8]), piece, 10);
	strcat(leds_line, piece);
	strcat(leds_line, " ");
	//num2hex(SIMP->IO_Registers[9], piece, 32);
	paddednum2hex(SIMP->IO_Registers[9], piece, 8);
	strcat(leds_line, piece);
	printf("%s\n", leds_line);
	Element* e = allocate();
	init_element(e, SIMP->PC, leds_line);
	add_element(leds, e);
}

void write_display(Processor* SIMP, Dictionary* display) {
	printf("display: ");
	char display_line[100] = "";
	char piece[12];
	_itoa((unsigned int)(SIMP->IO_Registers[8]), piece, 10);// careful
	strcat(display_line, piece);
	strcat(display_line, " ");
	//num2hex(SIMP->IO_Registers[10], piece, 32);
	paddednum2hex(SIMP->IO_Registers[10], piece, 8);
	strcat(display_line, piece);
	printf("%s\n", display_line);
	Element* e = allocate();
	init_element(e, SIMP->PC, display_line);
	add_element(display, e);
}

void irq2handler(Processor* SIMP) {
	int l = SIMP->irq2in->number_of_elements;
	int isIrq = 0;
	for (int i = 0; i < l; i++) {
		int n = atoi(SIMP->irq2in->list[i].line);
		if (SIMP->IO_Registers[8] == n) {
			isIrq = 1;
			break;
		}
	}
	SIMP->IO_Registers[5] = isIrq;
}



void execute_row(Processor* SIMP, Memory* memory, Harddisk* hd, Monitor* monitor, char* row, Dictionary* trace,
	Dictionary* hwregtrace, Dictionary* leds, Dictionary* display) {

	int opcode = get_opcode(row);
	int indices[4];
	get_indices(row, indices);//
	int immidiate[2];
	get_imm(row, immidiate);//
	insert_imm(SIMP, immidiate[0], immidiate[1]);
	// write trace line here.
	write_trace(SIMP, trace, row);
	switch (opcode)
	{
	case 0:
		add(indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 1:
		sub(indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 2:
		mac(indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 3:
		and (indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 4:
		or (indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 5:
		xor (indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 6:
		sll(indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 7:
		sra(indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 8:
		srl(indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 9:
		beq(indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 10:
		bne(indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 11:
		blt(indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 12:
		bgt(indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 13:
		ble(indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 14:
		bge(indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 15:
		jal(indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 16:
		lw(indices[0], indices[1], indices[2], indices[3], SIMP, memory);
		//printf(" loaded memory value %d",memory->)
		break;
	case 17:
		sw(indices[0], indices[1], indices[2], indices[3], SIMP, memory);
		printf(" stored memory value %d\n", memory->Memory[SIMP->Registers[indices[1]] + SIMP->Registers[indices[1]]]);
		break;
	case 18:
		reti(indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 19:
		in(indices[0], indices[1], indices[2], indices[3], SIMP);
		// hwregtrace here
		write_hwregtrace(SIMP, hwregtrace, indices[1], indices[2], 0);
		break;
	case 20:
		out(indices[0], indices[1], indices[2], indices[3], SIMP);
		//hwregtrace here
		write_hwregtrace(SIMP, hwregtrace, indices[1], indices[2], 1);
		if (SIMP->Registers[indices[1]] + SIMP->Registers[indices[2]] == 9) {
			//leds here
			write_leds(SIMP, leds);
		}
		if (SIMP->Registers[indices[1]] + SIMP->Registers[indices[2]] == 10) {
			//display here
			write_display(SIMP, display);
		}
		break;
	case 21:
		halt(SIMP);
		break;
	default:
		break;
	}

	SIMP->IO_Registers[8]++; // clks++

	//hardware functions:
	timer_handler(SIMP); // timer.
	hard_disk_handler(memory, hd, SIMP); //hard disk operations this iteration.
	monitor_handler(monitor, SIMP);  //monitor operations this iteration.
	if (SIMP->hasJumped == 1) {
		SIMP->hasJumped = 0;
	}
	else {
		SIMP->PC++;
	}

}

void execute_code(Processor* SIMP, Memory* memory, Harddisk* hd, Monitor* monitor, Dictionary* trace,
	Dictionary* hwregtrace, Dictionary* leds, Dictionary* display) {

	char line[100];
	while (SIMP->Flag != 0) {
		printf("%d\n\n", SIMP->IO_Registers[8]);
		irq2handler(SIMP);
		get_line(SIMP->executable, SIMP->PC, line);
		printf("%s\n", line);
		execute_row(SIMP, memory, hd, monitor, line, trace, hwregtrace, leds, display);
		//irq2handler(SIMP);
		int irq = (SIMP->IO_Registers[0] && SIMP->IO_Registers[3])
			|| (SIMP->IO_Registers[1] && SIMP->IO_Registers[4])
			|| (SIMP->IO_Registers[2] && SIMP->IO_Registers[5]);


		if (irq && !SIMP->isHandlingInterrupt)
		{
			SIMP->IO_Registers[7] = SIMP->PC; //irqreturn = current PC
			SIMP->PC = SIMP->IO_Registers[6];
		}
	}
}


