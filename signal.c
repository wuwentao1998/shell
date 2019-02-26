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

// 如果通过管道连接多个进程，当前面进程终止时，最后一个进程却在无限循环，则会陷入死循环
// 解决方法：添加WNOHANG参数，如果没有已经停止的进程直接返回
// TODO: 为什么jobs还在，deleteJob没有使用吗
    while ((childPid = waitpid(-1, &status, WNOHANG)) > 0)
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

    flag++;
}

handler_t *Signal(int signum, handler_t *handler)
{
    struct sigaction action, old_action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
	unix_error("Signal error");
    return (old_action.sa_handler);
}


void unix_error(char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

// void sigpipe_handler()
// {
//     return;
// }



