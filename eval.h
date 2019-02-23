//
// Created by wwt on 10/4/18.
//

#ifndef UNTITLED_EVAL_H
#define UNTITLED_EVAL_H

#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdbool.h>
#include <unistd.h>

#include "job.h"
#include "macro.h"
#include "free.h"
#include "internalcmd.h"
#include "signal.h"


void eval(int cmdNum, int* argcNum, char*** argv, char* record);
pid_t Fork();
bool internalCmd(char*** argv, int cmdNum, int* argcNum, int k);
void redirection(int argc, char*** argv, int* out, int n, int cmdNum, int* argcNum);

#endif //UNTITLED_EVAL_H
