//
// Created by wwt on 10/4/18.
//
//#define DEBUG
#include "eval.h"

pid_t pid[MAXCMD] = { 0 };
pid_t pid2 = 0;
extern bool isBG;
extern struct job_t jobs[MAXJOBS];
extern sigset_t mask_all, prev_one, mask_one;

int flag = 0;

void eval(int cmdNum, int* argcNum, char*** argv, char* record)
{
    if (argv[0][0] == NULL)
        return;

    sigprocmask(SIG_BLOCK, &mask_one, &prev_one);

    int fd[MAXCMD][2] ;
    int k = -1;

    for (int i = 1; i < cmdNum; i++)
    {
        if (pipe(fd[i]) == -1)
        {
            fprintf(stderr, "can't set up pipe %d", i);
            fflush(stderr);
            freeArgv(argv, cmdNum, argcNum);
            exit(1);
        }
    }

    if(cmdNum == 1)
    {
        if (internalCmd(argv, cmdNum, argcNum, 0))
        {
            sigprocmask(SIG_SETMASK, &prev_one, NULL);
            return;
        }

        if ((pid2 = Fork()) == 0)
        {
            sigprocmask(SIG_SETMASK, &prev_one, NULL);
            int out[2] = { -1, -1 };
            redirection(argcNum[0], argv, out, 0, cmdNum, argcNum);

            if (argv[0][0] == NULL)
            {
                fprintf(stderr, "error: missing program\n");
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }

            if (execvp(argv[0][0], argv[0]) == -1)//第二个参数不确定正确
            {
                fprintf(stderr, "%s: command not found\n", argv[0][0]);
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }


            if (execvp(argv[0][0], argv[0]) == -1)//第二个参数不确定正确
            {
                fprintf(stderr, "%s: command not found\n", argv[0][0]);
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }
        }

        sigprocmask(SIG_BLOCK, &mask_all, NULL);
        addJob(jobs, record, pid2);
    }
    else
    {
        for (k = 0; k < cmdNum; k++)
            if ((pid[k] = Fork()) == 0)
                break;

        // 父进程在k = cmdNum 前保持循环，所以只有子进程才能通过 k == i
        for (int i = 0; i < cmdNum; i++)
        {
            if (k == i)
            {
                sigprocmask(SIG_SETMASK, &prev_one, NULL);
                for (int j = 1; j < cmdNum; j++)
                {
                    if (i == j)
                        close(fd[i][1]);
                    else if (j == (i + 1))
                        close(fd[j][0]);
                    else
                    {
                        close(fd[j][0]);
                        close(fd[j][1]);
                    }

                    if (i != cmdNum - 1)
                        dup2(fd[i + 1][1], STDOUT_FILENO);

                    if (i != 0)
                        dup2(fd[i][0], STDIN_FILENO);
                }

                if (internalCmd(argv, cmdNum, argcNum, k))
                {
                    freeArgv(argv, cmdNum, argcNum);
                    exit(0);
                }

                int out[2] = { -1, -1 };

                redirection(argcNum[k], argv, out, k, cmdNum, argcNum);

                if (argv[k][0] == NULL)
                {
                    fprintf(stderr, "error: missing program\n");
                    fflush(stderr);
                    freeArgv(argv, cmdNum, argcNum);
                    exit(1);
                }

                if (execvp(argv[k][0], argv[k]) == -1)//第二个参数不确定正确
                {
                    fprintf(stderr, "%s: command not found\n", argv[k][0]);
                    fflush(stderr);
                    freeArgv(argv, cmdNum, argcNum);
                    exit(1);
                }

            }
        }

        // 这一段属于父进程
        for(int i = 1; i < cmdNum; i++)
        {
            close(fd[i][0]);
            close(fd[i][1]);
        }

        for(int i = MAXCMD - 1; i >= 0; i--)
        {
            if (pid[i] == 0)
                continue;
            else
            {
                sigprocmask(SIG_BLOCK, &mask_all, NULL);
                addJob(jobs, record, pid[i]);
                break; // ?
            }
        }
    }

    if (!isBG)
    {
        while(flag == 0)
            sigsuspend(&prev_one);
    }

    sigprocmask(SIG_SETMASK, &prev_one, NULL);
    #ifdef DEBUG
        if(!isBG)
        {
            while (flag == 0)
            ;
            flag = 0;
        }
    #endif
}


pid_t Fork()
{
    pid_t pid;

    if ((pid = fork()) < 0)
    {
        fprintf(stderr, "fork_error");
        fflush(stderr);
    }

    return pid;
}


bool internalCmd(char*** argv, int cmdNum, int* argcNum, int k)
{
    char* command = argv[k][0];

    if (!strcmp(command, "exit"))
    {
        fprintf(stdout, "exit\n");
        fflush(stdout);
        freeArgv(argv, cmdNum, argcNum);
        exit(0);
    }
    if (!strcmp(command, "cd"))
    {
        char* dir = argv[k][1];
        if (argv[k][2] != 0)
        {
            fprintf(stderr, "cd : too many arguments\n");
            fflush(stderr);
        }
        else
            cd(dir);

        return 1;
    }
    if (!strcmp(command, "pwd"))
    {
        int out[2] = { -1, -1 };
        redirection(argcNum[k], argv, out, k, cmdNum, argcNum);
        pwd();

        if (out[0] != -1)
            dup2(out[0], STDIN_FILENO);

        if (out[1] != -1)
            dup2(out[1], STDOUT_FILENO);

        return 1;
    }
    if (!strcmp(command, "jobs"))
    {
        int out[2] = { -1, -1 };
        redirection(argcNum[k], argv, out, k, cmdNum, argcNum);
        listJobs(jobs);

        if (out[0] != -1)
            dup2(out[0], STDIN_FILENO);

        if (out[1] != -1)
            dup2(out[1], STDOUT_FILENO);

        return 1;
    }

    return 0;
}


void redirection(int argc, char*** argv, int* out, int n, int cmdNum, int* argcNum)
{

    bool isredirction[2] = {false, false};//0 for < , 1 for > or >>
    bool judge = false;
    if (cmdNum == 1) // yu dao > > > zen me ban?
    {
        for (int m = 0; m < argc; m++)
            if (strcmp(argv[n][m], "pwd") == 0)
            {
                judge = true;
                break;
            }
    }

    out[0] = dup(STDIN_FILENO);
    out[1] = dup(STDOUT_FILENO);
    for (int i = 0; i < argc; i++)
    {
        if(argv[n][i] == NULL)
            continue;

        if (strcmp(argv[n][i], ">>") == 0)
        {

            if (n!= cmdNum - 1)
            {
                fprintf(stderr, "error: duplicated output redirection\n");
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }

            if (i == argc - 1)
            {
                fprintf(stderr, "syntax error near unexpected token `|\'\n");
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }

            if (strcmp(argv[n][1+i], ">") == 0 || strcmp(argv[n][1+i], ">>") == 0 ||
                strcmp(argv[n][1+i], "<") == 0)
            {
                fprintf(stderr, "syntax error near unexpected token `%s\'\n", argv[n][i+1]);
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }

            if(isredirction[1] != true)
                isredirction[1] = true;
            else
            {
                fprintf(stderr, "error: duplicated output redirection\n");
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }

            if (!judge)
            {
                free(argv[n][i]);
                argv[n][i] = NULL;
            }

            int fd = open(argv[n][i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);

            if (dup2(fd, STDOUT_FILENO) < 0)
            {
                fprintf(stderr, "/dev/permission_denied: Permission denied\n");
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }
            if (!judge)
            {
                free(argv[n][i+1]);
                argv[n][i+1] = NULL;
            }

        }
        else if (strcmp(argv[n][i], ">") == 0)
        {
            if (i == argc - 1)
            {
                fprintf(stderr, "syntax error near unexpected token `|\'\n");
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }

            if (n!= cmdNum - 1)
            {
                fprintf(stderr, "error: duplicated output redirection\n");
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }

            if (strcmp(argv[n][1+i], ">") == 0 || strcmp(argv[n][1+i], ">>") == 0 ||
                strcmp(argv[n][1+i], "<") == 0)
            {

                fprintf(stderr, "syntax error near unexpected token `%s\'\n", argv[n][i+1]);
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }

            if(isredirction[1] != true)
                isredirction[1] = true;
            else
            {
                fprintf(stderr, "error: duplicated output redirection\n");
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }

            if (!judge)
            {
                free(argv[n][i]);
                argv[n][i] = NULL;
            }

            int fd = open(argv[n][i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (dup2(fd, STDOUT_FILENO) < 0)
            {
                fprintf(stderr, "/dev/permission_denied: Permission denied\n");
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }
            if (!judge)
            {
                free(argv[n][i+1]);
                argv[n][i+1] = NULL;
            }

        }
        else if (strcmp(argv[n][i], "<") == 0)
        {
            if (n!= 0)
            {
                fprintf(stderr, "error: duplicated input redirection\n");
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }

            if (i == argc - 1)
            {
                fprintf(stderr, "syntax error near unexpected token `|\'\n");
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }

            if (strcmp(argv[n][1+i], ">") == 0 || strcmp(argv[n][1+i], ">>") == 0 ||
                strcmp(argv[n][1+i], "<") == 0)
            {
                fprintf(stderr, "syntax error near unexpected token `%s\'\n", argv[n][i+1]);
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }

            if(isredirction[0] != true)
                isredirction[0] = true;
            else
            {
                fprintf(stderr, "error: duplicated input redirection\n");
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }

            if (!judge)
            {
                free(argv[n][i]);
                argv[n][i] = NULL;

            }

            int fd = open(argv[n][i + 1], O_RDONLY, 0644);
            if (fd == -1)
            {
                fprintf(stderr, "%s: No such file or directory\n", argv[n][i+1]);
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }

            if (dup2(fd, STDIN_FILENO) < 0)
            {
                fprintf(stderr, "/dev/permission_denied: Permission denied\n");
                fflush(stderr);
                freeArgv(argv, cmdNum, argcNum);
                exit(1);
            }
            if (!judge)
            {
                free(argv[n][i+1]);
                argv[n][i+1] = NULL;
            }
        }
    }

    if(!judge)
    {
        for (int i = 0; i < argc; i++)
        {
            if (argv[n][i] == NULL)
            {
                for(int j = i+1; j < argc ;j++)
                {
                    if(argv[n][j] != NULL)
                    {
                        argv[n][i] = argv[n][j];
                        argv[n][j] = NULL;
                        break;
                    }
                }
            }
        }
    }
}


