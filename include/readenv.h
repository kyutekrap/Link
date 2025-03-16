#ifndef READENV_H
#define READENV_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

struct Env {
    int debug;
};
typedef struct Env Env;

Env readenv(char *root);

#endif