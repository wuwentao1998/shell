//
// Created by wwt on 10/4/18.
//
//#define DEBUG
#include "signal.h"

extern pid_t pid[MAXCMD];
extern pid_t pid2;
extern int cmdNum;
extern bool isEval;
extern struct job_t jobs[MAXJOBS];

extern int flag;


void sigint_handler()
{

    if (isEval == false)
    {
        fprintf(stdout, "\n");
        fprintf(stdout, "mumsh $ ");
    }
    else
    {
        if (cmdNum > 1)
        {
            for (int i = 0; i < cmdNum; ++i)
		{
			while(pid[i] == 0)
				continue;

			kill(pid[i],SIGINT);
		}
        }
        else
        {
             while(pid2 == 0)
                continue;

            kill(pid2, SIGINT);
        }

        fprintf(stdout, "\n");
    }
    fflush(stdout);
}


void sigchld_handler()
{
    int status;
    pid_t childPid;
    sigset_t mask_all, prev_all;

    sigfillset(&mask_all);

    #ifdef DEBUG
        fprintf(stdout, "in sigchld_handler\n");
        fflush(stdout);
    #endif

    while ((childPid = waitpid(-1, &status, 0)) > 0)
    {
        for (int i = 0; i < MAXJOBS; ++i) {
            if(jobs[i].pid == childPid)
            {
                sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
                deleteJob(childPid);
                sigprocmask(SIG_SETMASK, &prev_all, NULL);
                break;
            }
        }
    }

    flag = 1;
}

void sigpipe_handler()
{

}



