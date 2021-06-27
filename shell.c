// Programmer: Aidan Williams, 260831434 
// Date: 2021
// Purpose: Virtual Shell to control the Simulated OS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shell.h"
#include "interpreter.h"

int parse(char input[]) {
    //variables to parse userInput into separate words
    char tmp[1000];
    //char **words = (char**)malloc(100 * sizeof(char*));
    char* words[100];
    int i = 0;
    int j;
    int w = 0;
    int numArgs = 0;
    int toReturn = 4;

    // clear any input before each pass
    fflush(stdin);

    //remove \n from input
    char* parsedInput = strtok(input, "\n");
    parsedInput = strtok(parsedInput, "\r");
    
    if (!parsedInput) {
        return 0;
    }
    while (parsedInput[i] != '\0' && i < 1000) {        //while not EOL feed input into temp variable
        
        for(i; parsedInput[i] == ' ' && i < 1000; i++); //skip any spaces at the beginning

        if (parsedInput[i] == '\0') {
            continue;
        }
        
        for (j = 0; parsedInput[i] != '\0' && parsedInput[i] != ' ' && i < 1000; i++, j++) {
            tmp[j] = parsedInput[i];
        }
        tmp[j] = '\0';
        words[w] = strdup(tmp);
        i++;
        w++;
        ++numArgs;
    }
    
    //make sure input not just spaces or other edge case where there are no words to interpret
    if(words[0]) {
        toReturn = interpreter(words, numArgs);
    }
    
    return toReturn;
}

void checkError(int errorCode) {
    if (errorCode == -1) {
        exit(99);
    }
    else if (errorCode == 1) {
        printf("Unknown Command\n");
    }
    else if (errorCode == 2) {
        printf("Variable does not exist\n");
    }
    else if (errorCode == 3) {
        printf("Script not found\n");
    }
    else if (errorCode == 6) {
        printf("ERROR: Not enough RAM to add program.\n");
    }
    else if (errorCode == 7) {
        printf("Too many scripts. Max is 3\n");
    }
    else if (errorCode == 8) {
        printf("ERROR: Failed to load file into BackingStore.\n");
    }
    else if (errorCode == 9) {
        printf("Exec command failed to open at least one file. \n");
    }
    else if (errorCode == 10) {
        printf("ERROR: File too large. Max size is 40 lines.\n");
    }
    else if (errorCode == 11) {
        printf("ERROR: Failed to load page.\n");
    }
    else if (errorCode == 12) {
        printf("ERROR: Failed to update page table.\n");
    }
}

int shellUI() {
    char prompt[100] = {'$', '\0'};
    char userInput[1000];
    int errorCode;

    printf("Welcome to Aidan Williams' shell!\n");
    printf("Version 3.0 created March 2021\n");
 
    while (1) {

        

        // only print prompt when stdin not redirected
        if (isatty(STDIN_FILENO)) {
            printf("%s ", prompt);
        }
        
        // check if EOF when stdin is redirected
        if(fgets(userInput, 999, stdin) == NULL) {
            break;
        }

        // check for empty return
        if (userInput[0] == 0x0A) {
            continue;
        }

        // error checking
        errorCode = parse(userInput);
        checkError(errorCode);

    }
    // exit with code 0
    return 0;
}

