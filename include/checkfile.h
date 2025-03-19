#ifndef CHECKFILE_H
#define CHECKFILE_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../macro/errcode.h"

struct File {
    int readDecor;
    int validFile;
    char *root;
    int errCode;
};
typedef struct File File;

File checkfile(const char *filename);

#endif