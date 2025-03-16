#ifndef CHECKFILE_H
#define CHECKFILE_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct File {
    int readDecor;
    int validFile;
    char *root;
};
typedef struct File File;

File checkfile(const char *filename);

#endif