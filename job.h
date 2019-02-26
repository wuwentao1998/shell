//
// Created by wwt on 10/13/18.
//

#ifndef PROJECT_JOB_H
#define PROJECT_JOB_H

#include "macro.h"

#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

struct job_t{
    pid_t pid;
    int jid;
    int state;
    char cmdline[MAXARGV];
};




void clearJob(struct job_t* job);
void initJobs(struct job_t* jobs);
void addJob(struct job_t* jobs, char* cmdline, pid_t lastPid);
void listJobs(struct job_t* jobs);
void deleteJob(pid_t id);
int maxjid(struct job_t *jobs);

#endif //PROJECT_JOB_H
