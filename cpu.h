#ifndef _CPU_
#define _CPU_

struct CPU {
    int IP;
    int offset;
    char IR[40];
    int quanta;
};

struct CPU* cpu;

int run_cpu(int quanta, struct PCB* p);

#endif