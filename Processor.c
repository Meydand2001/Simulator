#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Processor.h"
#include "hardware.h"
#include "NumberOperations.h"
#include "dictionary.h"


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

void init_memory(Memory* memory,Dictionary* dmemin) {
	for (int i = 0; i < 4096; i++) {
		if (i < dmemin->number_of_elements) {
			//memory->Memory[i]=translate(dmemin->list[i])
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

void init_Processor(Processor* SIMP, Dictionary* executable) {
	SIMP->executable = executable;
	for (int i = 0; i < 16; i++) {
		SIMP->Registers[i] = 0;
	}
	for (int i = 0; i < 23; i++) {
		SIMP->IO_Registers[i] = 0;
	}
	SIMP->PC = 0;
	SIMP->Flag = 1;
	SIMP->isHandlingInterrupt = 0;
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

voidand (int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	SIMP->Registers[rd_index] = (SIMP->Registers[rs_index] & SIMP->Registers[rt_index]) & SIMP->Registers[rm_index];
}

void or (int rd_index, int rs_index, int rt_index, int rm_index, Processor * SIMP)
{
	SIMP->Registers[rd_index] = (SIMP->Registers[rs_index] | SIMP->Registers[rt_index]) | SIMP->Registers[rm_index];
}

voidxor (int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	SIMP->Registers[rd_index] = (SIMP->Registers[rs_index] ^ SIMP->Registers[rt_index]) ^ SIMP->Registers[rm_index];
}

void sll(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
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
	if (SIMP->Registers[rs_index] == SIMP->Registers[rt_index])
		SIMP->PC = SIMP->Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
}

void bne(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	if (SIMP->Registers[rs_index] != SIMP->Registers[rt_index])
		SIMP->PC = SIMP->Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
}


void blt(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	if (SIMP->Registers[rs_index] < SIMP->Registers[rt_index])
		SIMP->PC = SIMP->Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
}


void bgt(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	if (SIMP->Registers[rs_index] > SIMP->Registers[rt_index])
		SIMP->PC = SIMP->Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
}


void ble(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	if (SIMP->Registers[rs_index] <= SIMP->Registers[rt_index])
		SIMP->PC = SIMP->Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
}


void bge(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	if (SIMP->Registers[rs_index] >= SIMP->Registers[rt_index])
		SIMP->PC = SIMP->Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
}

void jal(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	SIMP->Registers[rd_index] = SIMP->PC + 1;
	SIMP->PC = SIMP->Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
}

void lw(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP, Memory* memory)
{
	SIMP->Registers[rd_index] = memory->Memory[SIMP->Registers[rs_index] + SIMP->Registers[rt_index]] + SIMP->Registers[rm_index];
}

void sw(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP, Memory* memory)
{
	memory->Memory[SIMP->Registers[rs_index]] = SIMP->Registers[rm_index] + SIMP->Registers[rd_index];
}

void reti(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	SIMP->PC = SIMP->IO_Registers[7];
	SIMP->isHandlingInterrupt = 0; //no longer handling interrupt
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
	strncpy(op, row, 2);
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
	imm[0] = hex2num(imm1_hex);
	imm[1] = hex2num(imm2_hex);
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


void execute_row(Processor* SIMP, Memory* memory, Hard_disk* hd, Monitor* monitor, char* row, Dictionary* trace,
	Dictionary* hwregtrace, Dictionary* leds, Dictionary* display) {
	int opcode = get_opcode(row);
	int indices[4];
	get_indices(row, indices);//
	int immidiate[2];
	get_imm(row, immidiate);//
	insert_imm(SIMP, immidiate[0], immidiate[1]);
	// write trace line here.
	char trace_line[500]="";
	char hex[15];
	num2hex(SIMP->PC, hex, 3);

	strcat(trace_line,hex);
	strcat(trace_line," ");
	strcat(trace_line, row);
	for (int i = 0; i < 16; i++) {
		num2hex(SIMP->Registers[i], hex, 8);
		strcat(trace_line, " ");
		strcat(trace_line, hex);
	}
	Element* e = allocate();
	init_element(e, SIMP->PC, trace_line);
	add_element(trace, e);
	// trace ends here.

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
		break;
	case 17:
		sw(indices[0], indices[1], indices[2], indices[3], SIMP, memory);
		break;
	case 18:
		reti(indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 19:
		in(indices[0], indices[1], indices[2], indices[3], SIMP);
		// hwregtrace here
		char hwregtrace_line[200] = "";
		char piece[12];
		itoa((unsigned int)(SIMP->IO_Registers[8]), piece, 10);
		strcat(hwregtrace_line, piece);
		strcat(hwregtrace_line, " WRITE ");
		// index to name function.
		index2name(SIMP->Registers[indices[1]] + SIMP->Registers[indices[2]], piece);
		strcat(hwregtrace_line, piece);
		strcat(hwregtrace_line, " ");
		num2hex(SIMP->IO_Registers[SIMP->Registers[indices[1]] + SIMP->Registers[indices[2]]], piece, 8);
		strcat(hwregtrace_line, piece);
		Element* e = allocate();
		init_element(e, SIMP->PC, hwregtrace_line);
		add_element(hwregtrace, e);
		break;
	case 20:
		out(indices[0], indices[1], indices[2], indices[3], SIMP);
		//
		char hwregtrace_line[200] = "";
		char piece[12];
		itoa((unsigned int)(SIMP->IO_Registers[8]), piece, 10);
		strcat(hwregtrace_line, piece);
		strcat(hwregtrace_line, " READ ");
		// index to name function.
		index2name(SIMP->Registers[indices[1]] + SIMP->Registers[indices[2]], piece);
		strcat(hwregtrace_line, piece);
		strcat(hwregtrace_line, " ");
		num2hex(SIMP->IO_Registers[SIMP->Registers[indices[1]] + SIMP->Registers[indices[2]]], piece, 8);
		strcat(hwregtrace_line, piece);
		Element* e = allocate();
		init_element(e, SIMP->PC, hwregtrace_line);
		add_element(hwregtrace, e);
		if (SIMP->Registers[indices[1]]+SIMP->Registers[indices[2]] == 9) {
			char leds_line[100] = "";
			char piece[12];
			itoa((unsigned int)(SIMP->IO_Registers[8]), piece, 10);
			strcat(leds_line, piece);
			strcat(leds_line," ");
			num2hex(SIMP->IO_Registers[9], piece, 8);
			strcat(leds_line, piece);
			Element* e = allocate();
			init_element(e, SIMP->PC, leds_line);
			add_element(leds, e);
		}
		if (SIMP->Registers[indices[1]] + SIMP->Registers[indices[2]] == 10) {
			char display_line[100] = "";
			char piece[12];
			itoa((unsigned int)(SIMP->IO_Registers[8]), piece, 10);
			strcat(display_line, piece);
			strcat(display_line, " ");
			num2hex(SIMP->IO_Registers[10], piece, 8);
			strcat(display_line, piece);
			Element* e = allocate();
			init_element(e, SIMP->PC, display_line);
			add_element(display, e);
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

	int irq = (SIMP->IO_Registers[0] && SIMP->IO_Registers[3])
		|| (SIMP->IO_Registers[1] && SIMP->IO_Registers[4])
		|| (SIMP->IO_Registers[2] && SIMP->IO_Registers[5]);


	if (irq && !SIMP->isHandlingInterrupt)
	{
		SIMP->IO_Registers[7] = SIMP->PC; //irqreturn= current PC
		SIMP->PC = SIMP->IO_Registers[6];
	}

}

void execute_code(Processor* SIMP, Memory* memory, Hard_disk* hd, Monitor* monitor, Dictionary* trace,
	Dictionary* hwregtrace, Dictionary* leds, Dictionary* display) {
	while (SIMP->Flag != 0) {
		char line[100];
		get_line(SIMP->executable, SIMP->PC, line);

		execute_row(SIMP, memory, hd, monitor, line, trace, hwregtrace, leds, display);
	}
}

/*void main()
{
	printf("%d", and (12, 12, 8));
}
*/
