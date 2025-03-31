#ifndef GETFILES_H
#define GETFILES_H

#include "../macro/errcode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

struct GetFiles {
    int errCode;
    char **files;
    size_t fileCnt;
};
typedef struct GetFiles GetFiles;

GetFiles getfiles(const char *filename);

#endif