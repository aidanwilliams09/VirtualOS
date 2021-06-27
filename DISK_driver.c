#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DISK_driver.h"

extern struct PARTITION currentPartition;
extern struct FAT fat[20];

char* block_buffer;
char filename[100];
FILE* active_file_table[5];
FILE* partitionFile;
//char data[currentPartition.blocksize * currentPartition.totalblocks];
void findBlock(FILE* fp, int block);
int findFile(FILE* fp, int file);
void writeFAT(FILE* fp);
void writeToFAT(FILE* fp);
void readFromFAT(FILE* fp);

void initIO() {

    for (int i = 0; i < 20; i++) {
        fat[i].filename = (char*)malloc(256);
        fat[i].file_length = -1;
        for (int j = 0; j < 10; j++) {
            fat[i].blockPtrs[j] = -1;
        }
        fat[i].current_location = -1;
        fat[i].free = 1;
    }

    for (int i = 0; i < 5; i++) {
        active_file_table[i]  = NULL;
        active_file_table_owners[i] = -1;
    }

    currentPartition.blocksize = -1;
    currentPartition.totalblocks = -1;
    currentPartition.lastfreeblock = 0;
}

int partition(char* name, int blocksize, int totalblocks) {
    
    initIO();
    currentPartition.blocksize = blocksize;
    currentPartition.totalblocks = totalblocks;
    system("if [ ! -d ./PARTITION ]; then mkdir PARTITION; fi");
    char command[100];
    sprintf(filename, "./PARTITION/%s", name);
    sprintf(command, "touch %s", filename);
    system(command);
    
    partitionFile = fopen(filename, "w");

    if (partitionFile) {
        block_buffer = malloc(blocksize);
        
        writeToFAT(partitionFile);
        
        for (int i = 0; i < blocksize * totalblocks; i++) {
            fputs("0", partitionFile);
        }
        
        fclose(partitionFile);
        return 0;
    }
    return 1;
}

int mountFS(char* name) {
    
    initIO();
    char* filepath = "./PARTITION/";
    sprintf(filename, "%s%s", filepath, name);
    partitionFile = fopen(filename, "r");
    
    if (partitionFile){
    
        readFromFAT(partitionFile);
        for (int i = 0; i < 20; i++) {
            fat[i].current_location = 0;
        }
        fclose(partitionFile);
        return 0;
    }

    return 1;
}

int openFile(char* name) {

    int fileindex = -1;
    int empty = -1;
    // find empty cell or the index of the file
    for (int i = 0; i < 20; i++) {
        if (fat[i].free) {
            empty = i;
            break;
        }
        if (strcmp(fat[i].filename, name) == 0) {
            fileindex = i;
            break;
        }
    }
    // table is full and file not in it
    if (fileindex == -1 && empty == -1) {
        // fat full
        return -2;
    }
    // file not in table so initialize it
    if (fileindex == -1 && empty != -1) {
        fat[empty].filename = malloc(sizeof(500));
        strcpy(fat[empty].filename, name);
        fat[empty].file_length = 0;
        fat[empty].current_location = 0;
        fat[empty].free = 0;
        return empty;
    }
    
    int userindex = -1;
    // file in fat, look for it in active_file_table
    for (int i = 0; i < 5; i++) {
        if (active_file_table_owners[i] == fileindex) {
            userindex = i;
            break;
        }
    }
    // not in active_file table
    // check if table full and then add to active file table
    if (userindex == -1) {
        if (active_file_table_owners[4] != -1) {
            // ERROR
            return -2;
        }
        for (int i = 0; i < 5; i++) {
            if (!active_file_table[i]) {
                active_file_table[i] = fopen(filename, "r+");
                active_file_table_owners[i] = fileindex;
                return fileindex;
            }
        }
    }
    // in active_file_table, return index
    return active_file_table_owners[userindex];
}

char* readBlock(int file) {
    if (file < 0 || file > 19) {
        // ERROR
        return NULL;
    }

    int block = fat[file].blockPtrs[fat[file].current_location];
    if (block == -1) {
        return NULL;
    }
    FILE* fp = NULL;
    int found = findFile(fp, file);

    if (found == -1) {
        return NULL;
    }
    fp = active_file_table[found];
    // go to block to read
    findBlock(fp, block);
    // Read block
    int i = 0;
    for (; i < currentPartition.blocksize; i++) {
        char tmp = fgetc(fp);
        if (tmp == '0') {
            block_buffer[i] = '\0';
            break;
        }
        block_buffer[i] = tmp;
    }
    block_buffer[i] = '\0';
    fat[file].current_location++;
    return block_buffer;
}



int writeBlock(int file, char* data) {

    if (file < 0 || file > 19) {
        return -2;
    }
    if (currentPartition.lastfreeblock >= currentPartition.totalblocks) {
        return -2;
    }
    if (fat[file].current_location >= 10) {
        return -2;
    }

    FILE* fileP;
    int found = findFile(fileP, file);
    fileP = active_file_table[found];

    if (found == -1) {
        return -2;
    }
    findBlock(fileP, currentPartition.lastfreeblock);

    char c;

    for (int i = 0; i < currentPartition.blocksize; i++) {
        c = *(data+i);  
        if (c == 0)
            c = '0';
        fputc(c, fileP);
    }
    fat[file].blockPtrs[fat[file].current_location] = currentPartition.lastfreeblock;
    currentPartition.lastfreeblock++;
    fat[file].current_location++;
    fat[file].file_length++;
    if(fat[file].current_location < 10)
        fat[file].blockPtrs[fat[file].current_location] = -1;
    
    writeToFAT(fileP);
    
    fflush(fileP);

    return 0;
}

void writeToFAT(FILE* fp) {

    rewind(fp);
    fwrite(&currentPartition.blocksize, sizeof(currentPartition.blocksize), 1, fp);
    fwrite(&currentPartition.totalblocks, sizeof(currentPartition.totalblocks), 1, fp);
    fwrite(&currentPartition.lastfreeblock, sizeof(currentPartition.lastfreeblock), 1, fp);

    for (int i = 0; i < 20; i++) {
        fwrite(fat[i].filename, sizeof(fat[i].filename), 1, fp);
        fwrite(&fat[i].file_length, sizeof(fat[i].file_length), 1, fp);
        fwrite(&fat[i].blockPtrs, sizeof(fat[i].blockPtrs), 1, fp);
        fwrite(&fat[i].current_location, sizeof(fat[i].current_location), 1, fp);
        fwrite(&fat[i].free, sizeof(fat[i].free), 1, fp);
    }
}

void readFromFAT(FILE* fp) {

    rewind(fp);
    fread(&currentPartition.blocksize, sizeof(currentPartition.blocksize), 1, fp);
    fread(&currentPartition.totalblocks, sizeof(currentPartition.totalblocks), 1, fp);
    fread(&currentPartition.lastfreeblock, sizeof(currentPartition.lastfreeblock), 1, fp);

    for (int i = 0; i < 20; i++) {
        fread(fat[i].filename, sizeof(fat[i].filename), 1, fp);
        fread(&fat[i].file_length, sizeof(fat[i].file_length), 1, fp);
        fread(&fat[i].blockPtrs, sizeof(fat[i].blockPtrs), 1, fp);
        fread(&fat[i].current_location, sizeof(fat[i].current_location), 1, fp);
        fread(&fat[i].free, sizeof(fat[i].free), 1, fp);
    }
}

int findFile(FILE* fp, int file) {
    
    int found = -1;
    for (int i = 0; i < 5; i++) {
        if (active_file_table_owners[i] == file) {
            found = i;
            break;
        }
    }
    return found;
}

void findBlock(FILE* fp, int block) {

    int o = sizeof(struct PARTITION) + 20 * sizeof(struct FAT);
    o += currentPartition.blocksize * block;
    fseek(fp, o, SEEK_SET);
}