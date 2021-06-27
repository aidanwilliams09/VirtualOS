// Programmer: Aidan Williams, 260831434 
// Date: 2021
// Purpose: Virtual Shell Memory

#include <string.h>
#include <stdio.h>

struct SHELLVARIABLE {
    char* var;
    char* val;
};
struct SHELLVARIABLE shellMemory[100];

int numvars = 0;

int findVar(char* var) {
    for(int i = 0; i < numvars; i++) {
        if (strcmp(shellMemory[i].var, var) == 0) {
            return i;
        }
    }
    return -1;
}

void updateVar(int index, char* val) {
    shellMemory[index].val = strdup(val);
}

void addVar(char* var, char* val) {
    shellMemory[numvars].var = strdup(var);
    shellMemory[numvars].val = strdup(val);
    numvars++;
}

char* getVal(char* var) {
    int index = findVar(var);
    if (index != -1) {
        return shellMemory[index].val;
    }
    return NULL;
}

void setVar(char* var, char* val) {
    int index = findVar(var);
    if (index == -1) {
        addVar(var, val);
    }
    else {
        updateVar(index, val);
    }
}