#ifndef _PCB_
#define _PCB_

struct PCB {
    int PC;
    int PC_page;
    int PC_offset;
    int pages_max;
    int pageTable[10];
    int PID;
};

struct PCB* makePCB(int pagesMax);

int generatePID();

#endif