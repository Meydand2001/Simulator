#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Processor.h"
#include "Dictionary.h"
#include "Number_operations.h"


/****************Timer Functions*****************/

/*******************
Function: timer_handler
Input: Processor* SIMP
Operation: Handles timer during runtime, gives out interruption when
timercurrent reaches timermax.
*******************/
void timer_handler(Processor* SIMP) {

	if (SIMP->IO_Registers[11] != 0) {  //timerenable!=0.
		if (SIMP->IO_Registers[12] >= SIMP->IO_Registers[13]) { //timercurrent larger than (or equal) to timer max.
			SIMP->IO_Registers[3] = 1;  //irqstatus=1.
			SIMP->IO_Registers[12] = 0; //timercurrent=0.
		}
		else {
			SIMP->IO_Registers[12]++; //timercurrent++.
		}
	}

}

/****************Monitor Functions*****************/


/*******************
Function: allocatemonitor
Input: None
Operation: Allocate a new monitor from heap memory.
Returns null if it has not found enough memory.
*******************/
Monitor* allocatemonitor() {
	Monitor* monitor = (Monitor*)malloc(sizeof(Monitor));
	if (monitor == NULL) {
		return NULL;
	}
	return monitor;
}

/*******************
Function: init_monitor
Input: Monitor* monitor
Operation: Initiate monitor
*******************/
void init_monitor(Monitor* monitor) {
	for (int i = 0; i < 256 * 256; i++) {
		monitor->pixels[i] = 0;
	}
}

/*******************
Function: write_pixel
Input: Monitor* monitor, int address, int value
Operation: Sets pixel's value to 'value'.
*******************/
void write_pixel(Monitor* monitor, int address, int value) {
	monitor->pixels[address] = value;
}

/*******************
Function: monitor_handler
Input: Monitor* monitor, Processor* SIMP
Operation: Handles monitor operations: if monitorcmd==1, writes monitordata to monitoraddr.
*******************/
void monitor_handler(Monitor* monitor, Processor* SIMP)
{
	if (SIMP->IO_Registers[22] == 1) //if monitorcmd==1
	{
		write_pixel(monitor, SIMP->IO_Registers[20], SIMP->IO_Registers[21]);  //write monitordata to monitoraddr
		SIMP->IO_Registers[22] = 0;  //monitorcmd=0
	}
}

/****************Hard Disk Functions*****************/

/*******************
Function: allocatedisk
Input: None
Operation: Allocate a new HardDisk from heap memory.
Returns null if it has not found enough memory.
*******************/
HardDisk* allocatedisk() {
	HardDisk* hd = (HardDisk*)malloc(sizeof(HardDisk));
	if (hd == NULL) {
		return NULL;
	}
	return hd;
}

/*******************
Function: init_hard_disk
Input: HardDisk* hd, Dictionary* diskin
Operation: Initiates HardDisk hd with memory given in diskin.
*******************/
void init_hard_disk(HardDisk* hd, Dictionary* diskin) {
	for (int i = 0; i < 16384; i++) { //disk size=16384=(128 sectors)*(128 words [word=4 bytes])
		if (i < diskin->number_of_elements) {
			hd->disk[i] = signedhex2num(diskin->list[i].line, 32);  //sets i'th elementh in disk to the numerical value
																	//of line i in diskin.
		}
		else {
			hd->disk[i] = 0; //if diskin is smaller than max size, set redundant values to 0. 
		}
	}
	hd->sector_size = 128; 
}


/*******************
Function: read_sector
Input: Memory* memory, HardDisk* hd, int sector, int buffer
Operation: Reads sector 'sector' from hard disk to address 'buffer' in memory.
*******************/
void read_sector(Memory* memory, HardDisk* hd, int sector, int buffer) {
	for (int i = 0; i < hd->sector_size; i++) {
		memory->Memory[buffer + i] = hd->disk[hd->sector_size * sector + i];
	}
}

/*******************
Function: write_sector
Input: Memory* memory, HardDisk* hd, int sector, int buffer
Operation: Writes from address 'buffer' in memory to sector 'sector' in hard disk.
*******************/
void write_sector(Memory* memory, HardDisk* hd, int sector, int buffer) {
	for (int i = 0; i < hd->sector_size; i++) {
		hd->disk[hd->sector_size * sector + i] = memory->Memory[buffer + i];
	}
}

/*******************
Function: hard_disk_handler
Input: Memory* memory, HardDisk* hd, Processor* SIMP
Operation: handles hard disk operations: reads/writes depending on diskcmd,
irq1status=1 when finished.
*******************/
void hard_disk_handler(Memory* memory, HardDisk* hd, Processor* SIMP)
{
	int cmd = SIMP->IO_Registers[14];
	int disk_status = SIMP->IO_Registers[17];
	if (disk_status == 0) {
		switch (cmd)
		{
		case 1: //if cmd==1, read from hard disk
		{
			read_sector(memory, hd, SIMP->IO_Registers[15], SIMP->IO_Registers[16]);
			SIMP->IO_Registers[17] = 1; //diskstatus=1;
			hd->cyclesSinceStart = 1;
		} break;
		case 2: //if cmd==2, write to hard disk
		{
			write_sector(memory, hd, SIMP->IO_Registers[15], SIMP->IO_Registers[16]);
			SIMP->IO_Registers[17] = 1; //diskstatus=1;
			hd->cyclesSinceStart = 1;
		} break;

		default:
			break;
		}

	}
	else
	{
		if (hd->cyclesSinceStart >= 1024) { //if 1024 cycles have passed from the start of an operation
			SIMP->IO_Registers[17] = 0; //diskstatus=0;
			SIMP->IO_Registers[14] = 0; //diskcmd=0;
			SIMP->IO_Registers[4] = 1; //irq1status=1;
		}
		else
			hd->cyclesSinceStart++;
	}

}

/****************Memory Functions*****************/

/*******************
Function: allocate_mem
Input: None
Operation: Allocate a new Memory (struct) from heap memory.
Returns null if it has not found enough memory.
*******************/
Memory* allocate_mem() {
	Memory* memory = (Memory*)malloc(sizeof(Memory));
	if (memory == NULL) {
		return NULL;
	}
	return memory;
}

/*******************
Function: destroy_mem
Input: Memory* memory
Operation: Frees allocated memory.
*******************/
void destroy_mem(Memory* memory) {
	free(memory);
}

/*******************
Function: init_memory
Input: Memory* memory, Dictionary* dmemin
Operation: Initiates memory with data given in dmemin.
*******************/
void init_memory(Memory* memory, Dictionary* dmemin) {
	for (int i = 0; i < 4096; i++) { //4096=depth of memory.
		if (i < dmemin->number_of_elements) {
			memory->Memory[i] = signedhex2num(dmemin->list[i].line, 32); //row i of memory=numerical value of i'th row of dmemin.
		}
		else {
			memory->Memory[i] = 0;  //if dmeming is shorter than 4096, pad memory with 0's.
		}
	}
}

/****************Processor Functions*****************/

/*******************
Function: allocate_pro
Input: None
Operation: Allocate a new Processor (struct) from heap memory.
Returns null if it has not found enough memory.
*******************/
Processor* allocate_pro() {
	Processor* SIMP = (Processor*)malloc(sizeof(Processor));
	if (SIMP == NULL) {
		return NULL;
	}
	return SIMP;
}

/*******************
Function: destroy_pro
Input: Processor* SIMP
Operation: Frees allocated Processor.
*******************/
void destroy_pro(Processor* SIMP) {
	destroydict(SIMP->executable);
	free(SIMP);
}

/*******************
Function: init_Processor
Input: Processor* SIMP, Dictionary* executable, Dictionary* irq2in
Operation: Initiates Processor SIMP with instruction memory from executable, irq2in memory from irq2in.
initiates all registers to 0.
initiate PC to 0.
*******************/
void init_Processor(Processor* SIMP, Dictionary* executable, Dictionary* irq2in) {
	SIMP->executable = executable;
	SIMP->irq2in = irq2in;
	for (int i = 0; i < 16; i++) {
		SIMP->Registers[i] = 0;
	}
	for (int i = 0; i < 23; i++) {
		SIMP->IO_Registers[i] = 0;
	}
	SIMP->PC = 0; 
	SIMP->Flag = 1; //1 until 'halt' instruction turns it to zero.
	SIMP->isHandlingInterrupt = 0; //at the start, not handeling interrupt
	SIMP->hasJumped = 0; //at the start, has not yet jumped.

}

/*******************Assembely functions******************/

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

void or(int rd_index, int rs_index, int rt_index, int rm_index, Processor * SIMP)
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
	SIMP->Registers[rd_index] = SIMP->Registers[rs_index] >> SIMP->Registers[rt_index];
}

void srl(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	int shift = SIMP->Registers[rt_index];
	int source = SIMP->Registers[rs_index];
	if (source >= 0) {
		SIMP->Registers[rd_index] = source >> shift;
	}
	else {
		int mask = -1;
		mask = mask << (32 - shift); //mask[0:31-shift]=0, mask[31-shift+1:31]=1
		SIMP->Registers[rd_index] = (source >> shift) ^ mask; //used xor to cancel out the ones at the start
															  //due to using >>
	}
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
	SIMP->Registers[rd_index] = memory->Memory[SIMP->Registers[rs_index] + SIMP->Registers[rt_index]] + SIMP->Registers[rm_index];
}

void sw(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP, Memory* memory)
{
	int n = SIMP->Registers[rs_index] + SIMP->Registers[rt_index];
	int d = SIMP->Registers[rm_index] + SIMP->Registers[rd_index];
	memory->Memory[n] = d;
}

void reti(int rd_index, int rs_index, int rt_index, int rm_index, Processor* SIMP)
{
	SIMP->PC = SIMP->IO_Registers[7]; //PC=irqreturn;
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
	SIMP->Flag = 0; //flag=0 to stop processor.
}



/*******************
Function: insert_imm
Input: Processor* SIMP, int imm1, int imm2
Operation: Inserts 'imm1','imm2' into their corresponding Registers.
also changes $zero back in case it was changed.
*******************/
void insert_imm(Processor* SIMP, int imm1, int imm2) {
	SIMP->Registers[0] = 0; // changes $zero back in case it was changed.
	SIMP->Registers[1] = imm1;
	SIMP->Registers[2] = imm2;
}

/*******************
Function: get_opcode
Input: char* row
Operation: Outputs opcode (int) corresponding to instruction 'row' (in hex).
*******************/
int get_opcode(char* row) {
	char op[20];
	strncpy(op, row, 2); //copies leading 2 digits from row to op.
	op[2] = '\0';
	int opcode = hex2num(op); //converts op to an integer.
	return opcode;
}

/*******************
Function: get_indices
Input: char* row, int indices[]
Operation: Places the indicies of rd, rs, rt, rm given in instruction 'row'
into an array 'indices[]' of size 4.
*******************/
void get_indices(char* row, int indices[]) {
	char rd_hex, rs_hex, rt_hex, rm_hex;
	rd_hex = row[2];       //hex value of index of rd.
	rs_hex = row[3];       //hex value of index of rs.
	rt_hex = row[4];       //hex value of index of rt.
	rm_hex = row[5];       //hex value of index of rm.
	indices[0] = hex2digit(rd_hex);  //converts rd_hex to integer, places it in indices[0].
	indices[1] = hex2digit(rs_hex);  //converts rs_hex to integer, places it in indices[1].
	indices[2] = hex2digit(rt_hex);  //converts rt_hex to integer, places it in indices[2].
	indices[3] = hex2digit(rm_hex);  //converts rm_hex to integer, places it in indices[3].
}

/*******************
Function: get_imm
Input: char* row, int imm[]
Operation: Places the values of imm1, imm2 given in instruction 'row'
into an array 'imm[]' of size 2.
*******************/
void get_imm(char* row, int imm[]) {
	char imm1_hex[4];
	imm1_hex[3] = '\0';
	char imm2_hex[4];
	imm2_hex[3] = '\0';
	for (int i = 0; i < 3; i++)
	{
		imm1_hex[i] = row[i + 6]; //[6:8] hex digits from row go into imm1_hex
		imm2_hex[i] = row[i + 9]; //[9:11] hex digits from row go into imm2_hex
	}
	imm[0] = signedhex2num(imm1_hex, 12); //converts imm1_hex to integer (using 12 binary bits); places to imm[0].
	imm[1] = signedhex2num(imm2_hex, 12); //converts imm2_hex to integer (using 12 binary bits); places to imm[1].
}

/*******************
Function: index2name
Input: int index, char* name
Operation: Writes the name of the IO_register with index 'index' to string 'name'.
*******************/
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
		strcpy(name, "error");  //if index was out of bounds, name="error".
		break;
	}

}

/*******************
Function: write_trace
Input: Processor* SIMP, Dictionary* trace, char* row
Operation: Writes Dictionary* trace during runtime.
*******************/
void write_trace(Processor* SIMP, Dictionary* trace, char* row) {
	char trace_line[500] = ""; //initiate trace_line
	char hex[15];
	paddednum2hex(SIMP->PC, hex, 3); //hex=PC in hex, padded to 3 digits.
	strcat(trace_line, hex);
	strcat(trace_line, " ");
	strcat(trace_line, row); //trace_line= "[PC in hex] [Instruction in hex]"
	for (int i = 0; i < 16; i++) { //adds [Register[i] in hex] to trace_line.
		paddednum2hex(SIMP->Registers[i], hex, 8);
		strcat(trace_line, " ");
		strcat(trace_line, hex);
	}
	//add trace_line to dictionary:
	Element* e = allocate();
	init_element(e, SIMP->PC, trace_line);
	add_element(trace, e);
}

/*******************
Function: write_hwregtrace
Input: Processor* SIMP, Dictionary* hwregtrace, int index1, int index2, int type
Operation: Writes Dictionary* hwregtrace during runtime.
type==1 -> WRITE.
type==0 -> READ.
*******************/
void write_hwregtrace(Processor* SIMP, Dictionary* hwregtrace, int index1, int index2, int type) {
	char hwregtrace_line[200] = "";
	char piece[12];
	sprintf(piece, "%u", SIMP->IO_Registers[8]); //piece = "[clks in decimal]"
	strcat(hwregtrace_line, piece);
	if (type == 1) { //if type==1, instruction was write (out).
		strcat(hwregtrace_line, " WRITE ");
	}
	else { //else (meaning type==0), instruction was read (in).
		strcat(hwregtrace_line, " READ ");
	}
	//now hwregtrace_line = "[clks in decimal] [WRITE/READ] "
	index2name(SIMP->Registers[index1] + SIMP->Registers[index2], piece); //name of IO Register with index rs+rt.
	strcat(hwregtrace_line, piece);
	//now hwregtrace_line = "[clks in decimal] [WRITE/READ] [NAME]"
	strcat(hwregtrace_line, " ");
	char data[15];
	paddednum2hex(SIMP->IO_Registers[SIMP->Registers[index1] + SIMP->Registers[index2]], data, 8);
	//data=rs+rt in hex padded with zeros to 8 digits.
	strcat(hwregtrace_line, data);
	//now hwregtrace_line = "[clks in decimal] [WRITE/READ] [NAME] [DATA]"
	//add hwregtrace_line to dictionary:
	Element* e = allocate();
	init_element(e, SIMP->PC, hwregtrace_line);
	add_element(hwregtrace, e);
}

/*******************
Function: write_leds
Input: Processor* SIMP, Dictionary* leds
Operation: Writes Dictionary* leds during runtime.
*******************/
void write_leds(Processor* SIMP, Dictionary* leds) {
	printf("leds: ");
	char leds_line[100] = "";
	char piece[12];
	sprintf(piece, "%u", SIMP->IO_Registers[8]); //piece = "[clks in decimal]"
	strcat(leds_line, piece);
	strcat(leds_line, " ");
	paddednum2hex(SIMP->IO_Registers[9], piece, 8); //converts leds to an 8-digit hex string.
	strcat(leds_line, piece);
	//leds_line="[clks in decimal] [leds in hex]"
	//add leds_line to dictionary:
	Element* e = allocate();
	init_element(e, SIMP->PC, leds_line);
	add_element(leds, e);
}

/*******************
Function: write_display
Input: Processor* SIMP, Dictionary* display
Operation: Writes Dictionary* display during runtime.
*******************/
void write_display(Processor* SIMP, Dictionary* display) {
	char display_line[100] = "";
	char piece[12];
	sprintf(piece, "%u", SIMP->IO_Registers[8]); //piece = "[clks in decimal]"
	strcat(display_line, piece);
	strcat(display_line, " ");
	paddednum2hex(SIMP->IO_Registers[10], piece, 8); //converts display7seg to an 8-digit hex string.
	strcat(display_line, piece);
	//display_line="[clks in decimal] [display7seg in hex]"
	//add display_line to dictionary:
	Element* e = allocate();
	init_element(e, SIMP->PC, display_line);
	add_element(display, e);
}

/*******************
Function: irq_handler
Input: Processor* SIMP
Operation: Handles irq2 interruption.
*******************/
void irq2handler(Processor* SIMP) {
	int l = SIMP->irq2in->number_of_elements; //number of rows in irq2in
	int isIrq = 0;
	int n;
	for (int i = 0; i < l; i++) {//searchs for line in irq2 with the current clock
		n = atoi(SIMP->irq2in->list[i].line); //n=i'th row of irq2 converted to int.
		if (SIMP->IO_Registers[8] == n) {
			isIrq = 1; //if there should be an interruption this clock, isIrq=1.
			break;
		}
	}
	SIMP->IO_Registers[5] = isIrq; //irq2status=isIrq;
}


/*******************
Function: execute_row
Input: Processor* SIMP, Memory* memory, HardDisk* hd, Monitor* monitor, char* row, Dictionary* trace,
	Dictionary* hwregtrace, Dictionary* leds, Dictionary* display
Operation: Executes current row.
*******************/
void execute_row(Processor* SIMP, Memory* memory, HardDisk* hd, Monitor* monitor, char* row, Dictionary* trace,
	Dictionary* hwregtrace, Dictionary* leds, Dictionary* display) {

	int opcode = get_opcode(row); //get opcode of row.
	int indices[4];
	get_indices(row, indices); //get indices of registers for current instruction.
	int immidiate[2];
	get_imm(row, immidiate); //get immidiates from row
	insert_imm(SIMP, immidiate[0], immidiate[1]); //insert immidiates into processor
	write_trace(SIMP, trace, row); //add trace of this clock
	switch (opcode) //apply instruction for this opcode.
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
		write_hwregtrace(SIMP, hwregtrace, indices[1], indices[2], 0);  //add hwregtrace of this clock if in was issued
		break;
	case 20:
		out(indices[0], indices[1], indices[2], indices[3], SIMP);
		write_hwregtrace(SIMP, hwregtrace, indices[1], indices[2], 1); //add hwregtrace of this clock if out was issued
		if (SIMP->Registers[indices[1]] + SIMP->Registers[indices[2]] == 9) {
			//if leds register was accessed, add to leds dictionary.
			write_leds(SIMP, leds);
		}
		if (SIMP->Registers[indices[1]] + SIMP->Registers[indices[2]] == 10) {
			//if display register was accessed, add to display dictionary.
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
	if (SIMP->hasJumped == 1) { //if there was a jump, turn off hasJumped
		SIMP->hasJumped = 0;
	}
	else { //if there wasn't a jump, PC++.
		SIMP->PC++; 
	}

}

/*******************
Function: execute_code
Input: Processor* SIMP, Memory* memory, HardDisk* hd, Monitor* monitor, char* row, Dictionary* trace,
	Dictionary* hwregtrace, Dictionary* leds, Dictionary* display
Operation: Executes entire assembly code.
*******************/
void execute_code(Processor* SIMP, Memory* memory, HardDisk* hd, Monitor* monitor, Dictionary* trace,
	Dictionary* hwregtrace, Dictionary* leds, Dictionary* display) {

	char line[100];
	while (SIMP->Flag != 0) {
		irq2handler(SIMP); //check for irq2 this clock

		get_line(SIMP->executable, SIMP->PC, line); //line=instruction of this PC
		execute_row(SIMP, memory, hd, monitor, line, trace, hwregtrace, leds, display);

		int irq = (SIMP->IO_Registers[0] && SIMP->IO_Registers[3])
			|| (SIMP->IO_Registers[1] && SIMP->IO_Registers[4])
			|| (SIMP->IO_Registers[2] && SIMP->IO_Registers[5]); //irq=irq of this clock

		int resultirq = irq && !SIMP->isHandlingInterrupt; //resultirq =0 if handeling interrupt, =irq if not.

		if (resultirq) //if not handeling interrupt and irq==1:
		{
			SIMP->isHandlingInterrupt = 1;
			SIMP->IO_Registers[7] = SIMP->PC; //irqreturn = current PC
			SIMP->PC = SIMP->IO_Registers[6]; //PC=irqhandler.
		}
	}
}
