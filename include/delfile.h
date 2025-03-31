#ifndef DELFILE_H
#define DELFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../macro/errcode.h"

struct DelFile {
    int errCode;
    char *file;
};
typedef struct DelFile DelFile;

DelFile delfile(const char *filename);

#endif