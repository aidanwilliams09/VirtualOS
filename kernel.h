#ifndef _KERNEL_
#define _KERNEL_

struct NODE {
    struct PCB* pcb;
    struct NODE* next;
};

struct READYQUEUE {
    struct NODE* head;
    struct NODE* tail;
};

struct READYQUEUE* queue;

struct PCB* myinit(int pagesMax);

int scheduler();

#endif