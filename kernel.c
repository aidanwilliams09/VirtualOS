// Programmer: Aidan Williams, 260831434 
// Date: 2021
// Purpose: Simulated Kernel that controls the ReadyQueue and schedules processes

#include <stdio.h>
#include <stdlib.h>

#include "shell.h"
#include "pcb.h"
#include "ram.h"
#include "cpu.h"
#include "kernel.h"

extern struct CPU* cpu;
extern struct READYQUEUE* queue;
extern char* ram[40];


// create a new node with a pcb and add it to the end
void addToReady(struct PCB* newPCB) {

    struct NODE *newNode = (struct NODE*)malloc(sizeof(struct NODE));
    newNode->pcb = newPCB;
    newNode->next = NULL;
    
    if (queue->head == NULL) {
        queue->head = newNode;
        queue->tail = newNode;
    }
    else {
        queue->tail->next = newNode;
        queue->tail = newNode;
    }
}

// return the head of the readyqueue
struct NODE* getNextPCB() {

    struct NODE* nextNode = queue->head;
    queue->head = nextNode->next;
    nextNode->next = NULL;
    return nextNode;
}

int scheduler() {

    // while the queue is not empty
    while(queue->head) {
        struct NODE* node = getNextPCB();
        struct PCB* pcb = node->pcb;
        free(node);

        // set cpu IP to the pcb PC
        cpu->IP = pcb->PC;
        int quanta = cpu->quanta;
        
        // run until end of program, page fault, or for cpu->quanta
        int status = run_cpu(quanta, pcb);
        
        // remove from ram and free the pcb if program is done
        if (status == -2) {
            removeFromRAM(pcb);
            free(pcb);
        }
        // add it back to ready list
        else {
            addToReady(pcb);
        }
    }
    return 0;
}

// remove all processes from Queue
void clearReadyQueue() {

    while (queue->head) {
        struct NODE* curNode = queue->head;
        queue->head = queue->head->next;
        free(curNode);
    }
}

// helper method to create a pcb and add it to the ready list
struct PCB* myinit(int pagesMax) {

    struct PCB* pcb = makePCB(pagesMax);
    addToReady(pcb);
    return pcb;
}

// initialize ram and create BackingStore
void boot() {

    for(int i = 0; i < 40; i++) {
        ram[i] = NULL;
    }
    system("rm -rf BackingStore");
    system("mkdir BackingStore");
}

// create the ready queue and set quanta
// call shellUI
int kernel() {

    queue = (struct READYQUEUE*)(malloc(sizeof(struct READYQUEUE)));
    queue->head = NULL;
    queue->tail = NULL;
    cpu = (struct CPU*)(malloc(sizeof(struct CPU)));
    cpu->quanta = 2;
    int errorCode = shellUI();

    return errorCode;
}

int main() {

    int errorCode = 0;
    boot();
    errorCode = kernel();
    return errorCode;
}