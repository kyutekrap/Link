#ifndef READINC_H
#define READINC_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "../macro/errcode.h"

struct ReadInc {
    char **files;
    int errCode;
};
typedef struct ReadInc ReadInc;

ReadInc readinc(char *root);

#endif