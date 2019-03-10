//
// Created by wwt on 10/4/18.
//

#include "free.h"


void freeCmd(char* cmd[MAXCMD], int cmdNum)
{
    for(int i = 0; i < cmdNum; i++)
        free(cmd[i]);
}

void clearBuffer(void)
{
    //fprintf(stdout," ");//pay attention!
    int temp;
    while ((temp = getchar()) != EOF && temp != '\n')
        continue;
}

void freeArgv(char*** argv, int cmdNum, int* argcNum)
{
    for (int i = 0; i < cmdNum; i++)
        for (int j = 0; j < argcNum[i]; j++)
            free(argv[i][j]);

    for (int i = 0; i < cmdNum; i++)
        free(argv[i]);

    free(argcNum);
    free(argv);
}




