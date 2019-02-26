//
// Created by wwt on 10/4/18.
//


#include "internalcmd.h"

void pwd()
{
    char buf[MAXLINE] = { 0 };
    char *pwd = getcwd(buf, MAXLINE);
    if (pwd == NULL)
    {
        fprintf(stderr, "The path is too long.");
        fflush(stderr);

    }
    else
    {
        fprintf(stdout, "%s\n", buf);
        fflush(stdout);

    }
}

void cd(char* dir)
{
    if ( dir == NULL|| strcmp(dir, "") == 0 || strcmp(dir, "~") == 0)
    {
        struct passwd* user = getpwuid(getuid());
        char* home = (char*)malloc(strlen(user->pw_dir) * sizeof(char) + 1);
        strcpy(home, user->pw_dir);
        int fd = open(home, O_RDONLY);
        if (fd < 0)
        {
            fprintf(stderr, "%s: No such file or directory\n", dir);
            fflush(stderr);
        }
        else if (fchdir(fd) < 0)
        {
            fprintf(stderr, "%s: No such file or directory\n", dir);
            fflush(stderr);
        }
        free(home);
    }
    else
    {
        int fd = open(dir, O_RDONLY);
        if (fd < 0)
        {
            fprintf(stderr, "%s: No such file or directory\n", dir);
            fflush(stderr);
        }
        else if (fchdir(fd) < 0)
        {
            fprintf(stderr, "%s: No such file or directory\n", dir);
            fflush(stderr);
        }
    }
}
