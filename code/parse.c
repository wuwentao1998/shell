//
// Created by wwt on 10/4/18.
//

#include "parse.h"


void parse(char*** argv, char *cmd[MAXCMD], int* argcNum)
{
    for (int i = 0; i< cmdNum; i++)
        argcNum[i] = parseLine(argv, cmd[i], i);

    freeCmd(cmd, cmdNum);
}

int parseLine(char*** arg, char* cmd, int i)
{
    bool isDoubleQuation = false;
    bool isSingleQuation = false;
    char* pt = cmd;
    while ((*pt != '\0') && (*pt == ' '))
        pt++;
    char localCopy[MAXARGV] = {0};
    int localNum = 0;
    arg[i] = (char**)calloc(MAXARGC , sizeof(char*));
    int argcNum = 0;


    while (*pt != '\0')
    {
        if(*pt == '\"')
        {
            if(isDoubleQuation == false && isSingleQuation == false)
            {
                isDoubleQuation = true;
                pt++;
                continue;
            }
            else if(isDoubleQuation == true)
            {
                isSingleQuation = false;
                isDoubleQuation = false;
                pt++;
                continue;
            }
        }
        else if(*pt == '\'')
        {
            if (isDoubleQuation == false && isSingleQuation == false)
            {
                isSingleQuation = true;
                pt++;
                continue;
            }
            else if(isSingleQuation == true)
            {
                isSingleQuation = false;
                isDoubleQuation = false;
                pt++;
                continue;
            }

        }

        if ((*pt) == ' ' && isDoubleQuation == false && isSingleQuation == false)
        {
            localCopy[localNum] = '\0';
            localNum = 0;
            arg[i][argcNum] = (char*)malloc((strlen(localCopy)+ 1)* sizeof(char));
            strcpy(arg[i][argcNum++], localCopy);
            memset(localCopy, 0, sizeof(localCopy));
            pt++;
            while ((*pt != '\0') && (*pt == ' '))
                pt++;
            continue;
        }

        localCopy[localNum++] = (*pt);
        pt++;
    }
    if(localNum != 0)
    {
        localCopy[localNum] = '\0';
        arg[i][argcNum] = (char*)malloc((strlen(localCopy)+1)* sizeof(char));
        strcpy(arg[i][argcNum++], localCopy);
    }
    return argcNum;

}




