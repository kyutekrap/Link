#ifndef DELFILE_H
#define DELFILE_H

#include <string.h>
#include <stdlib.h>
#include "../macro/errcode.h"

struct DelFile {
    int errCode;
    char *file;
};
typedef struct DelFile DelFile;

DelFile delfile(const char *filename);

#endif