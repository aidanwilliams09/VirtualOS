// Programmer: Aidan Williams, 260831434 
// Date: 2021
// Purpose: Simulated RAM capable of holding parts of a program

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ram.h"
#include "pcb.h"

extern char *ram[40];
int nextAvailable = 0;

// add a line to ram at start
int addToRAM(char* line, int start) {
    ram[start] = strdup(line);
    return 0;
}

// clear all lines related to PCB p
void removeFromRAM(struct PCB* p) {
    for (int i = 0; i < 10; i++) {
        if (p->pageTable[i] != -1) {
            for (int j = 0; j < 4; j++) {
                ram[i * 4 + j] = NULL;
            }
        }
    }
}

int getNextAvailable() {
    for (int i = 0; i < 40; i += 4) {
        if (ram[i] == NULL) {
            return i;
        }
    }
    return -1;
}