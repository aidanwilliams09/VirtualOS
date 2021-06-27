// Programmer: Aidan Williams, 260831434 
// Date: 2021
// Purpose: Simulated Interpreter used to parse shell commands

#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "shellmemory.h"
#include "kernel.h"
#include "memorymanager.h"
#include "DISK_driver.h"

int run(char* words[], int numArgs) {

    int errorCode = 0;
    char line[1000];
    // check args
    if (numArgs != 2) {
        errorCode = 1;
    }
    else {
        //try to open the file
        FILE *f = fopen(words[1], "rt");

        if (f) {
            while (fgets(line, 999, f)) {
                errorCode = parse(line);
                if (errorCode != 0) {
                    break;
                }
            }
            fclose(f);
        }
        // file DNE
        else {
            errorCode = 3;
        }
    }
    
    return errorCode;
}

int print(char* words[], int numArgs) {
    // get value with error checking on var DNE
    char* val = getVal(words[1]);
    if (val != NULL && numArgs == 2) {
        printf("%s\n", val);
        return 0;
    }
    else if (numArgs != 2) {
        return 1;
    }
    else {
        return 2;
    }
}

int set(char* words[], int numArgs) {
    if (numArgs == 3) {
        setVar(words[1], words[2]);
        return 0;
    }
    else {
        return 1;
    }
}

int quit(char* words[], int numArgs) {
    // arg checks
    if (numArgs == 1) {
        return -1;
    }
    else {
        return 1;
    }
}

void printHelp() {

    printf("quit\t\t\t\tExit/terminate the shell\n");
    printf("set VAR STRING\t\t\tset VAR in shell memory\n");
    printf("print VAR\t\t\tprint value of VAR\n");
    printf("run SCRIPT.TXT\t\t\trun the script named SCRIPT.TXT\n");
    printf("exec script1 script2 script3\texecute up to 3 scripts\n");
}

int help(char* words[], int numArgs) {
    // arg checks
    if (numArgs == 1) {
        printHelp();
        return 0;
    }
    else {
        return 1;
    }
}

int exec(char* words[], int numArgs) {

    int errorCode = 0;
    // make sure not too many arguments
    if (numArgs > 4 || numArgs < 2) {
        errorCode = 7;
    }
    // init all programs
    else {
        for(int i = 1; i < numArgs; i++) {
            FILE* f = fopen(words[i], "r");
            if(f) {
                errorCode = launcher(f);
            }
            else {
                printf("failed on file %s\n", words[i]);
                errorCode = 9;
            }
        }
    }
    // if no error opening programs, schedule and run them
    if (errorCode == 0) {
        errorCode = scheduler();
    }

    return errorCode;
}

int mount(char* words[], int numArgs) {
    
    int errorCode = 0;
    if (numArgs != 4) {
        errorCode = 1;
    }
    else{
        char command[100];
        sprintf(command, "./PARTITION/%s", words[1]);
        FILE* fp = fopen(command, "r");
        if(!fp) {
            errorCode = partition(words[1], atoi(words[2]), atoi(words[3]));
        }
        else {
            fclose(fp);
        }
        if (errorCode == 0) {
            errorCode = mountFS(words[1]);
        }
    }
    return errorCode;
}

int write(char* words[], int numArgs) {
    
    int errorCode = 0;
    int fileindex = openFile(words[1]);
    fileindex = openFile(words[1]);
    if (fileindex == -2) {
        printf("Failed to open file.\n");
        return 0;
    }
    char* buffer[1000];
    char* p = strdup(words[2]);
    p++;
    
    if (numArgs == 3) {
        p[strlen(p) - 1] = 0;
    }
    buffer[0] = p;

    for (int i = 3; i < numArgs; i++) {
        if (i = numArgs - 1) {
            p = strdup(words[i]);
            p[strlen(p) - 1] = 0;
            buffer[i - 2] = p;
        }
        else {
            buffer[i - 2] = strdup(words[i]);
        }
    }
    char writebuffer3[100];
    sprintf(writebuffer3, "%s", buffer[0]);
    
    for (int i = 1; i < numArgs - 2; i++) {
        sprintf(writebuffer3, "%s %s ", writebuffer3, buffer[i]);
    }
 
    errorCode = writeBlock(fileindex, writebuffer3);
    if (errorCode == -2) {
        printf("Failed to write.\n");
        return 0;
    }
    return errorCode;
}

int read(char* words[], int numArgs) {

    int errorCode = 0;
    int fileindex = openFile(words[1]);
    if (fileindex == -2) {
        printf("Failed to open file.\n");
        return 0;
    }
    char* line = readBlock(fileindex);
    if (line) {
        char* buffer[3];
        buffer[0] = "set";
        buffer[1] = words[2];
        buffer[2] = line;
        errorCode = set(buffer, 3);
    }
    
    return errorCode;
}

int interpretCMD(char* words[], int numArgs) {

    int errorCode = 0;
    if (strcmp(words[0], "run") == 0) {
        errorCode = run(words, numArgs);
    }
    else if (strcmp(words[0], "print") == 0) {
        errorCode = print(words, numArgs);
    }
    else if(strcmp(words[0], "set") == 0) {
        errorCode = set(words, numArgs);
    }
    else if(strcmp(words[0], "quit") == 0) {
        errorCode = quit(words, numArgs);
    }
    else if(strcmp(words[0], "help") == 0) {
        errorCode = help(words, numArgs);
    }
    else if(strcmp(words[0], "exec") == 0) {
        errorCode = exec(words, numArgs);
    }
    else if(strcmp(words[0], "mount") == 0) {
        errorCode = mount(words, numArgs);
    }
    else if(strcmp(words[0], "read") == 0) {
        errorCode = read(words, numArgs);
    }
    else if(strcmp(words[0], "write") == 0) {
        errorCode = write(words, numArgs);
    }
    else {
        errorCode = 1;
    }
    return errorCode;
}

int interpreter(char* words[], int numArgs) {
    
    int errorCode = interpretCMD(words, numArgs);
    return errorCode;
}
