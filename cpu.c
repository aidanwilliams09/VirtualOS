// Programmer: Aidan Williams, 260831434 
// Date: 2021
// Purpose: Simulated CPU that runs programs

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ram.h"
#include "shell.h"
#include "pcb.h"
#include "cpu.h"
#include "memorymanager.h"

extern struct CPU* cpu;
extern char* ram[40];

// helper method to generate an interrupt and load the next page
int generateInterrupt(struct PCB* p) {
    p->PC_page++;
    // program is finished
    if (p->PC_page >= p->pages_max) {
        return -2;
    }

    int frame = p->pageTable[p->PC_page];

    // page fault, need to load new page into ram
    if(frame == -1) {

        // get file path
        char PID[32];    
        sprintf(PID, "%d", p->PID);
        char* filename = strcat(PID, ".txt");
        char filepath[1000] = "./BackingStore/";
        strcat(filepath, filename);

        FILE* f = fopen(filepath, "r");

        if (f) {
            resolvePage(p, p->PC_page, f);
            fclose(f);
        }
        else {
            printf("File %s does not exist.\n", filepath);
        }
    }

    // reset PC_offset and get new PC
    p->PC = p->pageTable[p->PC_page]*4;
    p->PC_offset = 0;
 
    return 0;
}

// run quanta instructions
int run_cpu(int quanta, struct PCB* p) {
    int errorCode = 0;
    int status = 0;
    int offset = p->PC_offset;

    for (int i = 0; i < quanta; i++) {
        
        // make sure ram is not null at location IP+offset
        // make sure no page fault
        if (offset >= 4 || ram[cpu->IP + offset] == NULL || cpu->IP + offset >= 40 || p->pageTable[p->PC_page] == -1) {
            status = generateInterrupt(p);
            return status;
        }
        
        else {
            strncpy(cpu->IR, ram[cpu->IP + offset], 40);
            errorCode = parse(strdup(cpu->IR));
            checkError(errorCode);
            offset++;
        }
    }
    p->PC_offset = offset;

    return status;
}