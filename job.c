//
// Created by wwt on 10/13/18.
//

#include "job.h"

int nextjid = 1;
bool isBG = false;
struct job_t jobs[MAXJOBS];

void clearJob(struct job_t* job)
{
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEFINE;
    job->cmdline[0] = '\0';
}

void initJobs(struct job_t* jobs)
{
    isBG = false;

    for (int i = 0; i < MAXJOBS; i++)
    {
        clearJob(&jobs[i]);
    }
}

void addJob(struct job_t* jobs, char* cmdline, pid_t lastPid)
{
    for (int i = 0; i < MAXJOBS; ++i)
    {
        if(jobs[i].jid == 0)
        {
            jobs[i].pid  = lastPid;
            jobs[i].jid = nextjid++;
            jobs[i].state = RUNNING;
            strcpy(jobs[i].cmdline, cmdline);
            fprintf(stdout, "[%d] %s\n", jobs[i].jid, jobs[i].cmdline);
            break;
        }
    }
}

void listJobs(struct job_t* jobs)
{
    for (int i = 0; i < MAXJOBS; i++)
    {
        if(jobs[i].jid != 0)
        {
            switch (jobs[i].state)
            {
                case RUNNING:
                    fprintf(stdout, "[%d] running %s\n", jobs[i].jid, jobs[i].cmdline);
                    break;
                case DONE:
                    fprintf(stdout, "[%d] done %s\n", jobs[i].jid, jobs[i].cmdline);
                    break;
                default:
                    break;
            }
        }
    }
}


