#ifndef GETDECOR_H
#define GETDECOR_H

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "../macro/errcode.h"

enum IsDecor {
    yes,
    no,
    null
};
typedef enum IsDecor IsDecor;

struct Decorator {
    IsDecor isDecor;
    int errCode;
    char **files;
};
typedef struct Decorator Decorator;

enum KeyT {
    none,
    function,
    argument,
    confirm,
    end
};
typedef enum KeyT KeyT;

Decorator getdecor(char *line);

#endif