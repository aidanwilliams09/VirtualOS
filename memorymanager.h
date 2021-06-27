#ifndef MEM_MANAGER
#define MEM_MANAGER

#include <stdio.h>

void resolvePage(struct PCB* pcb, int page, FILE* f);

int launcher(FILE* f);

#endif