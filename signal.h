//
// Created by wwt on 10/4/18.
//

#ifndef UNTITLED_SIGNAL_H
#define UNTITLED_SIGNAL_H
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include<stdio.h>
#include<sys/stat.h>
#include<dirent.h>
#include <unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#include "macro.h"
#include "job.h"

void sigint_handler();
void sigchld_handler();
void sigpipe_handler();
#endif //UNTITLED_SIGNAL_H
