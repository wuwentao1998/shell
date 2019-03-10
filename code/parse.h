//
// Created by wwt on 10/4/18.
//

#ifndef UNTITLED_PARSE_H
#define UNTITLED_PARSE_H
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdbool.h>
#include <unistd.h>


#include "macro.h"
#include "free.h"
#include "job.h"
extern int cmdNum;

void parse(char*** argv, char *cmd[MAXCMD], int* argcNum);
int parseLine(char*** argv, char* cmd, int i);

#endif //UNTITLED_PARSE_H
