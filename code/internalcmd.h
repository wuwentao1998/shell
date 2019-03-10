//
// Created by wwt on 10/4/18.
//

#ifndef UNTITLED_INTERNALCMD_H
#define UNTITLED_INTERNALCMD_H
#include<stdio.h>
#include<sys/stat.h>
#include<dirent.h>
#include <unistd.h>
#include<stdlib.h>
#include<string.h>
#include <fcntl.h>
#include<sys/types.h>
#include <sys/wait.h>
#include <pwd.h>

#include "macro.h"

void cd(char* dir);
void pwd();


#endif //UNTITLED_INTERNALCMD_H
