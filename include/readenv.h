#ifndef READENV_H
#define READENV_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "../macro/errcode.h"

struct Env {
    int debug;
    int errCode;
};
typedef struct Env Env;

Env readenv(char *root);

#endif