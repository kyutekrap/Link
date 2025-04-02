#ifndef GETDECO_H
#define GETDECO_H

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <stdio.h>
#include "../macro/errcode.h"

enum DecoType {
    import,
    debug,
    param
};
typedef enum DecoType DecoType;

struct GetDeco {
    int errCode;
    int isDeco;
    DecoType decoType;
    char *value;
};
typedef struct GetDeco GetDeco;

GetDeco getdeco(char *fline);

#endif