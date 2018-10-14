//
// Created by wwt on 10/4/18.
//

#ifndef UNTITLED_FREE_H
#define UNTITLED_FREE_H

#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdbool.h>
#include <unistd.h>

#include "macro.h"

void freeCmd(char* cmd[MAXCMD], int cmdNum);
void clearBuffer(void);
void freeArgv(char*** argv, int cmdNum, int* argcNum);

#endif //UNTITLED_FREE_H
