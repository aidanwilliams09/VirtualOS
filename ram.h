#ifndef _RAM_
#define _RAM_

#include <stdio.h>
#include "pcb.h"

int addToRAM(char* line, int start);

void removeFromRAM(struct PCB* p);

int getNextAvailable();

char *ram[40];

#endif