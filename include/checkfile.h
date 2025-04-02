#ifndef CHECKFILE_H
#define CHECKFILE_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../macro/errcode.h"

struct File {
    int validFile;
    char *root;
    int errCode;
    char *fname;
};
typedef struct File File;

File checkfile(const char *filename);

#endif