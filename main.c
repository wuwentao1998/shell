//
// Created by wwt on 10/4/18.
//

#include "parse.h"
#include "eval.h"
//#define DEBUG

bool isEval = false;
int cmdNum = 0;

extern struct job_t jobs[MAXJOBS];
extern bool isBG;
extern int flag;

int main(void)
{
    initJobs(jobs);
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchld_handler);
    signal(SIGPIPE, sigpipe_handler);

label1:
    while (true)
    {
        isBG = false;
        isEval = false;
        char *cmd[MAXCMD] = {0};
        cmdNum = 0;
        int recordNum = 0;
        char cmdline[MAXLINE] = {0};
        char record[MAXLINE] = {0};
        int ch;
        bool isredirction[2] = {false, false};//0 for < , 1 for > or >>
        bool isDoubleQuation = false;
        bool isSingleQuation = false;
        flag = 0;

        fprintf(stdout, "mumsh $ ");
        fflush(stdout);

        if ((ch = fgetc(stdin)) != EOF)
        {
            if (ch == '\n')
                continue;

            int num = 0;
            cmdline[num++] = (char) ch;
            record[recordNum++] = (char) ch;
            if (ch == '\'')
            {
                isSingleQuation = true;
                goto label5;
            }
            else if (ch == '\"')
            {
                isDoubleQuation = true;
                goto label4;
            }

            if (ch == '>')
                isredirction[1] = true;
            else if(ch == '<')
                isredirction[0] = true;

            if(isredirction[1] == true || isredirction[0] == true)
            {
                int tem = 0;
                ch = fgetc(stdin);
                if ( ch == '\n')
                {
                    while (ch == '\n' || ch == ' ')
                    {
                        if (ch == '\n')
                        {
                            fprintf(stdout, "> ");
                            fflush(stdout);
                        }
                        else if (tem == 0)
                        {
                            cmdline[num++] = ' ';
                            tem++;
                        }

                        ch = fgetc(stdin);
                    }
                }
                else if ( ch == ' ')
                {
                    while (ch == '\n' || ch == ' ')
                    {
                        if (ch == '\n')
                        {
                            fprintf(stdout, "> ");
                            fflush(stdout);
                        }
                        else if (tem == 0)
                        {
                            cmdline[num++] = ' ';
                            tem++;
                        }
                        record[recordNum++] = (char)ch;
                        ch = fgetc(stdin);
                    }
                }

                goto label2;
            }


            while ((ch = fgetc(stdin)) != '\n')
            {
            label2:
                if (ch == EOF)
                    continue;

                if(num == 1 && (cmdline[0] == '<' || (cmdline[0] == '>' && ch != '>')))
                    cmdline[num++] = ' ';

                if (ch == '\'')
                {
                    if(isDoubleQuation == false && isSingleQuation == false)
                        isSingleQuation = true;
                    else if(isSingleQuation == true)
                    {
                        isSingleQuation = false;
                        isDoubleQuation = false;
                    }
                }
                else if (ch == '\"')
                {
                    if(isDoubleQuation == false && isSingleQuation == false)
                        isDoubleQuation = true;
                    else if(isDoubleQuation == true)
                    {
                        isSingleQuation = false;
                        isDoubleQuation = false;
                    }
                }

                if(isDoubleQuation == true)
                {
                    cmdline[num++] = (char)ch;
                    record[recordNum++] = (char)ch;
                    label4:
                    while((ch = fgetc(stdin)) != '\"')
                    {
                        if (ch == '\n')
                        {
                            fprintf(stdout, "> ");
                            fflush(stdout);
                        }
                        cmdline[num++] = (char)ch;
                        record[recordNum++] = (char)ch;
                    }
                    isSingleQuation = false;
                    isDoubleQuation = false;
                    cmdline[num++] = '\"';
                    record[recordNum++] = '\"';
                    continue;
                }

                if(isSingleQuation == true)
                {
                    cmdline[num++] = (char)ch;
                    record[recordNum++] = (char)ch;
                    label5:
                    while((ch = fgetc(stdin)) != '\'')
                    {
                        if (ch == '\n')
                        {
                            fprintf(stdout, "> ");
                            fflush(stdout);
                        }
                        cmdline[num++] = (char)ch;
                        record[recordNum++] = (char)ch;
                    }
                    isSingleQuation = false;
                    isDoubleQuation = false;
                    cmdline[num++] = '\'';
                    record[recordNum++] = '\'';
                    continue;
                }


                if (ch == '|')
                {
                    cmdline[num++] = '\0';
                    record[recordNum++] = '|';
                    int len = (int) strlen(cmdline);
                    cmd[cmdNum] = (char *) malloc((len + 1) * sizeof(char) + 1);
                    strcpy(cmd[cmdNum++], cmdline);
                    memset(cmdline, 0, sizeof(cmdline));
                    isredirction[0] = false;
                    isredirction[1] = false;
                    num = 0;
                    ch = fgetc(stdin);
                    while (ch == '\n' || ch == ' ')
                    {
                        if (ch == '\n')
                        {
                            fprintf(stdout, "> ");
                            fflush(stdout);
                        }
                        record[recordNum++] = (char) ch;
                        ch = fgetc(stdin);
                    }
                    if (ch == '|')
                    {
                        fprintf(stderr, "error: missing program\n");
                        fflush(stderr);
                        clearBuffer();
                        freeCmd(cmd, cmdNum);
                        goto label1;
                    }
                    goto label2;
                }

                if ((ch == '>' || ch == '<'))
                {
                    if (cmdline[0] == '>' && num == 1)
                    {
                        cmdline[num++] = (char) ch;
                        record[recordNum++] = (char) ch;
                        ch = fgetc(stdin);
                        isredirction[1] = false;
                        goto label3;
                    }

                    cmdline[num++] = ' ';
                    cmdline[num++] = (char) ch;
                    record[recordNum++] = (char) ch;
                    ch = fgetc(stdin);
                    if (cmdline[num - 1] == '>' && ch == '>')
                    {
                        cmdline[num++] = (char) ch;
                        record[recordNum++] = (char) ch;
                        ch = fgetc(stdin);
                    }

                    label3:
                    cmdline[num++] = ' ';

                    if (cmdline[num - 2] == '>')
                    {
                        if (isredirction[1] == false)
                            isredirction[1] = true;
                        else
                        {
                            int i = num - 3;
                            if (cmdline[i] == '>')
                                i--;
                            for (; i >=0 ; i--)
                            {

                                if(cmdline[i] == ' ')
                                    continue;
                                else if (cmdline[i] == '>')
                                {
                                    fprintf(stderr, "syntax error near unexpected token `>\'\n");
                                    fflush(stderr);
                                    if(ch != '\n')
                                        clearBuffer();
                                    freeCmd(cmd, cmdNum);
                                    goto label1;
                                }
                                else
                                    break;
                            }


                        }
                    }

                    if (cmdline[num - 2] == '<')
                    {
                        if (isredirction[0] == false)
                            isredirction[0] = true;
                        else
                        {
                            for (int i = num - 3; i >=0 ; i--)
                            {
                                if(cmdline[i] == ' ')
                                    continue;
                                else if (cmdline[i] == '<')
                                {
                                    fprintf(stderr, "syntax error near unexpected token `<\'\n");
                                    fflush(stderr);
                                    if(ch != '\n')
                                        clearBuffer();
                                    freeCmd(cmd, cmdNum);
                                    goto label1;
                                }
                                else
                                    break;
                            }
                        }
                    }

                    while (ch == '\n' || ch == ' ')
                    {
                        if (ch == '\n')
                        {
                            fprintf(stdout, "> ");//why not need to print \n
                            fflush(stdout);
                        }
                        ch = fgetc(stdin);
                    }

                    goto label2;
                }

                cmdline[num++] = (char) ch;
                record[recordNum++] = (char) ch;
            }
            cmdline[num] = '\0';
            int len = (int) strlen(cmdline);
            cmd[cmdNum] = (char *) malloc((len + 1) * sizeof(char) + 1);
            strcpy(cmd[cmdNum++], cmdline);
        }
        else
        {
            fprintf(stdout, "exit\n");
            fflush(stdout);
            exit(0);
        }
	    isEval = true;


        int* argcNum = (int*)calloc(cmdNum , sizeof(int));
        char*** argv = (char***)calloc(cmdNum , sizeof(char**));

        int len = (int)strlen(cmd[cmdNum- 1]);
        int i = len - 1;
        while (cmd[cmdNum - 1][i] == '\n' || cmd[cmdNum - 1][i] == '\t' || cmd[cmdNum - 1][i] == ' ')
            i--;
        if (cmd[cmdNum - 1][i] == '&')
        {
            isBG = true;
            cmd[cmdNum - 1][i] = ' ';
        }

        parse(argv, cmd, argcNum, cmdNum);

        record[recordNum++] = '\0';

        eval(cmdNum, argcNum, argv, record);

        freeArgv(argv, cmdNum, argcNum);
    }
    return 0;
}




