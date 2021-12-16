#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Header.h"

struct Memory {
	int Memory[1024];
} memory;

struct Processor {
	int Registers[16];
	int IO_Registers[23];
	int PC;
	int Flag
} processor;



void add(int rd_index,int rs_index, int rt_index,int rm_index,struct Processor SIMP)
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

/*void main()
{
	printf("%d", and (12, 12, 8));
}
*/