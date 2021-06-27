#ifndef DISK
#define DISK

#include <stdio.h>
#include <stdlib.h>

struct PARTITION {
    int totalblocks;
    int blocksize;
    int lastfreeblock;
} currentPartition;

struct FAT {
    char *filename;
    int file_length;
    int blockPtrs[10];
    int current_location;
    int free;
} fat[20];

FILE* active_file_table[5];
int active_file_table_owners[5];

void initIO();

int partition(char* name, int blocksize, int totalblocks);

int mountFS(char* name);

int openFile(char* name);

char* readBlock(int file);

int writeBlock(int file, char* data);

#endif