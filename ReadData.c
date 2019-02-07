//
//  readshit.c
//  Discrete Event Simulator
//
//  Created by Zach Yodh on 2/6/19.
//  Copyright © 2019 Zach Yodh. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getNumColumns(char*, char);
ssize_t readline(char **, FILE *);
char* getValue(char*, char*, int, int);

int getNumColumns(char* line, char delimiter){
    int columns = 0;
    if(line == NULL){
        return columns;
    }
    columns++;
    
    while(*line){
        columns += (*(line++) == delimiter); //if char in line == delimiter it will add 1, otherwise it'll add 0
    }
    return columns;
}

ssize_t readline(char **lineptr, FILE *stream){
    size_t len = 0;  // Size of the buffer, ignored.
    
    ssize_t chars = getline(lineptr, &len, stream);
    
    if((*lineptr)[chars-1] == '\n') {
        //printf("Replacing newline character...\n");
        (*lineptr)[chars-1] = '\0';
        --chars;
    }
    
    return chars;
}

char* getValue(char* file, char* searchStr, int colSearch, int colResult) {
    FILE* fp;
    char *line = NULL, *temp = NULL;
    size_t len = 0;
    ssize_t chars;
    int numCols = 0, currCol = 0, searchFound = 0;
    
    //TRY TO OPEN FILE
    if((fp = fopen(file,"r")) == NULL){
        printf("ERROR opening file \"%s\"\n",file );
        return NULL;
    }
    
    //MAKE SURE THERE ARE LINES IN FILE
    //getline returns -1 on failure to read line
    //because getline call is in the first condiiton, line in getNumColumns wont be NULL, therefore, getNumColumns is successfully called
    if((chars = readline(&line,fp)) == -1 || (numCols = getNumColumns(line,' ')) == 0){
        return NULL;
    }
    //Make sure colSearch & colResult is within range (0, numCols)
    if(numCols <= colSearch || numCols <= colResult){
        printf("Requested search or result column index does not exist.\n");
    }
    do{
        //allocate enough memory to copy line
        char * tempLine = (char *) malloc((chars + 1) * sizeof(char));
        strcpy(tempLine, line);
        
        //locate the column to search at and check if it matches
        while((temp = strsep(&line," ")) != NULL && !searchFound){
            //compare strings only if the current column is the column to search
            if(currCol == colSearch){
                //printf("temp = %s.\n",temp); //debugging
                searchFound = (strcmp(temp,searchStr) == 0);
            }
            currCol++;
        }
        currCol = 0; //reset current column in aid to retrieve result
        //locate the column that matches colResult and return the string
        while((temp = strsep(&tempLine," "))!= NULL && searchFound){
            if(currCol == colResult){
                //printf("result = %s\n",temp);
                return temp;
            }
            currCol++;
        }
        
    }while((chars = readline(&line,fp) != -1)); //read new line of file
    printf("Search string not found....\n");
    return NULL;
}
