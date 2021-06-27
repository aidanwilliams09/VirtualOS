// Programmer: Aidan Williams, 260831434 
// Date: 2021
// Purpose: Blueprint for a PCB

#include <stdlib.h>

#include "pcb.h"
#include "ram.h"

int PID = 0;

// create a new pcb with default values
struct PCB* makePCB(int pagesMax) {

    struct PCB* newPCB = (struct PCB*)malloc(sizeof(struct PCB));
    newPCB->PC = getNextAvailable();
    newPCB->PC_offset = 0;
    newPCB->PC_page = 0;
    newPCB->pages_max = pagesMax;

    for (int i = 0; i < 10; i++) {
        newPCB->pageTable[i] = -1;
    }

    newPCB->PID = PID;
    PID++;

    return newPCB;
}

// helper method to get a new PID for a process
int generatePID() {
    return PID;
}
