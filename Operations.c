#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Operations.h"
#include "dictionary.h"

struct Memory {
	int Memory[1024];
} memory;

struct Processor {
	Dictionary* executable;
	int Registers[16];
	int IO_Registers[23];
	int PC;
	int Flag
} processor;



void add(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	SIMP.Registers[rd_index] = SIMP.Registers[rs_index] + SIMP.Registers[rt_index] + SIMP.Registers[rm_index];
}

void sub(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	SIMP.Registers[rd_index] = SIMP.Registers[rs_index] - SIMP.Registers[rt_index] - SIMP.Registers[rm_index];
}

void mac(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	SIMP.Registers[rd_index] = SIMP.Registers[rs_index] * SIMP.Registers[rt_index] + SIMP.Registers[rm_index];
}

void and(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	SIMP.Registers[rd_index] = (SIMP.Registers[rs_index] & SIMP.Registers[rt_index]) & SIMP.Registers[rm_index];
}

void or (int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	SIMP.Registers[rd_index] = (SIMP.Registers[rs_index] | SIMP.Registers[rt_index]) | SIMP.Registers[rm_index];
}

void xor(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	SIMP.Registers[rd_index] = (SIMP.Registers[rs_index] ^ SIMP.Registers[rt_index]) ^ SIMP.Registers[rm_index];
}

void sll(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	SIMP.Registers[rd_index] = SIMP.Registers[rs_index] << SIMP.Registers[rt_index];
}

void sra(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	SIMP.Registers[rd_index] = ~((~SIMP.Registers[rs_index]) >> SIMP.Registers[rt_index]);
}

void srl(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	SIMP.Registers[rd_index] = SIMP.Registers[rs_index] >> SIMP.Registers[rt_index];
}

void beq(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	if (SIMP.Registers[rs_index] == SIMP.Registers[rt_index])
		SIMP.PC = SIMP.Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
}

void bne(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	if (SIMP.Registers[rs_index] != SIMP.Registers[rt_index])
		SIMP.PC = SIMP.Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
}


void blt(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	if (SIMP.Registers[rs_index] < SIMP.Registers[rt_index])
		SIMP.PC = SIMP.Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
}


void bgt(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	if (SIMP.Registers[rs_index] > SIMP.Registers[rt_index])
		SIMP.PC = SIMP.Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
}


void ble(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	if (SIMP.Registers[rs_index] <= SIMP.Registers[rt_index])
		SIMP.PC = SIMP.Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
}


void bge(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	if (SIMP.Registers[rs_index] >= SIMP.Registers[rt_index])
		SIMP.PC = SIMP.Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
}

void jal(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	SIMP.Registers[rd_index] = SIMP.PC + 1;
	SIMP.PC = SIMP.Registers[rm_index] % Pow(2, 12);  //mod 2^12 to take the low [11:0] bits.
}

void lw(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP, struct Memory memory)
{
	SIMP.Registers[rd_index] = memory.Memory[SIMP.Registers[rs_index] + SIMP.Registers[rt_index]] + SIMP.Registers[rm_index];
}

void sw(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP, struct Memory memory)
{
	memory.Memory[SIMP.Registers[rs_index]] = SIMP.Registers[rm_index] + SIMP.Registers[rd_index];
}

void reti(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	SIMP.PC = SIMP.IO_Registers[7];
}

void in(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	SIMP.Registers[rd_index] = SIMP.IO_Registers[SIMP.Registers[rs_index] + SIMP.Registers[rt_index]];
}

void out(int rd_index, int rs_index, int rt_index, int rm_index, struct Processor SIMP)
{
	SIMP.IO_Registers[SIMP.Registers[rs_index] + SIMP.Registers[rt_index]] = SIMP.Registers[rm_index];
}

void halt(struct Processor SIMP)
{
	SIMP.Flag = 0;
}

void insert_imm(struct Processor SIMP, int imm1, int imm2) {
	SIMP.Registers[1] = imm1;
	SIMP.Registers[2] = imm2;
}


int get_opcode(char* row) {
	char* op;
	strncpy(op, row, 2);
	int opcode=hex2num(op);
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
	for (int i = 0;i < 3;i++)
	{
		imm1_hex[i] = row[i + 6];
		imm2_hex[i] = row[i + 9];
	}
	imm[0] = hex2num(imm1_hex);
	imm[1] = hex2num(imm2_hex);
}

void execute_row(struct Processor SIMP, struct Memory memory, char* row) {
	int opcode = get_opcode(row);
	int indices[4];
	get_indices(row, indices);
	int immidiate[2];
	get_imm(row, immidiate);
	insert_imm(SIMP, immidiate[0], immidiate[1]);
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
		break;
	case 20:
		out(indices[0], indices[1], indices[2], indices[3], SIMP);
		break;
	case 21:
		halt(SIMP);
		break;
	default:
		break;
	}
}

void execute_code(struct Processor SIMP, struct Memory memory) {
	while (SIMP.Flag != 0) {
		char line[100];
		get_line(SIMP.executable, SIMP.PC, line);
		execute_row(SIMP, memory, line);
	}
}

/*void main()
{
	printf("%d", and (12, 12, 8));
}
*/
