// Programmer: Aidan Williams, 260831434 
// Date: 2021
// Purpose: Simulated Memory Manager

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <time.h>

#include "pcb.h"
#include "ram.h"
#include "kernel.h"

extern struct READYQUEUE* queue;

int countTotalPages(FILE *f) {
    // make sure f points to top of file
    rewind(f);

    // read all lines are track count
    int lines = 0;
    char line[1000];
    while (fgets(line, 1000, f) != NULL) {
        lines++;
    }

    int pages = (lines-1) / 4 + 1;
    return pages;
}

// reads a page from f into ram[frameNumber]
void loadPage(int pageNumber, FILE *f, int frameNumber) {
    // make sure f points to beginning of f
    rewind(f);

    // skip over lines until at desired page
    char line[1000];
    int curPage = 0;
    while (curPage != pageNumber) {
        for (int i = 0; i < 4; i++) {
            fgets(line, 1000, f);
        }
        curPage++;
    }

    // read lines into ram frame
    for(int offset = 0; offset < 4; offset++) {
        ram[frameNumber * 4 + offset] = NULL;
        if (fgets(line, 1000, f) != NULL) {
            addToRAM(line, frameNumber * 4 + offset);
        }
    }

    rewind(f);
}

// returns index of first open frame
int findFrame() {

    int index = -1;

    for (int i = 0; i < 40; i += 4) {
        if (ram[i] == NULL) {
            index = i/4;
            break;
        }
    }
    return index;
}

// returns the index of a frame we are reassigning to p
int findVictim(struct PCB *p) {

    srand(time(0));

    int r = rand() % 10;
    int start = r;
    for (int i = 0; i < 10; i++) {
        if (p->pageTable[i] == r) {
            r = (r + 1) % 10;
            i = 0;
            if (r == start) {
                return -1;
            }
        }
    }

    return r;
}

// helper method to find page in victim frame
int findFrameInPCB(struct PCB* pcb, int victim) {

    int frame;

    for (int i=0; i<pcb->pages_max; i++) {
        if (pcb->pageTable[i] == victim) {
            frame = pcb->pageTable[i];
            return frame;
        }
    }
    return -1;
}

// helper method to update the victim PCB's page table
void updateVictimPCB(int victim) {

    struct NODE* temp = queue->head;

    while (temp) {
        int index = findFrameInPCB(temp->pcb, victim);
        if (index != -1) {
            temp->pcb->pageTable[index] = -1;
            break;
        }

        temp = temp->next;
    }
}

// updates the page table for p and a victim
int updatePageTable(struct PCB *p, int pageNumber, int frameNumber, int victimFrame) {

    int errorCode = 0;

    // no open frames, need to perform a swap
    if (victimFrame != -1 && frameNumber == -1) {
        updateVictimPCB(victimFrame);
        //printf("Updated victim PCB\n");
        p->pageTable[pageNumber] = victimFrame;      
    }

    // open frame at frameNumber
    else if (frameNumber != -1 && victimFrame == -1) {
        p->pageTable[pageNumber] = frameNumber;
    }

    // No frames that don't belong to p
    else if (frameNumber == -1 && victimFrame == -1) {
        errorCode = 12;
    }

    return errorCode;
}

// helper method to resolve page faults or loads
int resolvePage(struct PCB* pcb, int page, FILE* f) { 
    // try to find an open frame, if none available select victim
    int openFrame = findFrame();
    
    int openFlag = openFrame;
    int victimFlag = -1;

    if (openFlag == -1) {
        //printf("Finding victim\n");
        openFrame = findVictim(pcb);
        //printf("Found victim\n");
        if (openFrame == -1) {
            return 11;
        }
        victimFlag = openFrame;
    }

    if (pcb->PC == -1) {
        pcb->PC = openFrame * 4;
    }
    
    loadPage(page, f, openFrame);
    updatePageTable(pcb, page, openFlag, victimFlag);
}

int launcher(FILE *p) {
    // create a new file in the backing store to copy our text file into
    char filePath[1000] = "./BackingStore/";
    char PID[32], c;
    sprintf(PID, "%d", generatePID());
    const char* fileName = strcat(PID, ".txt");
    strcat(filePath, fileName);

    FILE* copyTo = fopen(filePath, "w+");

    // ensure file created successfully
    if (copyTo == NULL) {
        fclose(copyTo);
        return 8;
    }

    // copy to new file
    c = fgetc(p);
    while (c != EOF) {
        fputc(c, copyTo);
        c = fgetc(p);
    }

    fclose(p);
    fclose(copyTo);

    // reopen the file with read permission
    FILE* f = fopen(filePath, "r");

    int pages = countTotalPages(f);
    if (pages > 10) {
        return 10;
    }

    // create a new pcb and then perform operations to add pages into RAM
    struct PCB* pcb = myinit(pages);

    // only read first two pages into RAM
    if (pages > 2) {
        pages = 2;
    }

    int errorCode = 0;

    for (int i = 0; i < pages; i++) {
        errorCode = resolvePage(pcb, i, f);
    }
    
    fclose(f);

    return errorCode;
}