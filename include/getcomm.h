#ifndef GETCOMM_H
#define GETCOMM_H

#include "../macro/errcode.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

enum CommDirection {
    commNeutral,
    commStart,
    commEnd
};
typedef enum CommDirection CommDirection;

struct GetComm {
    int errCode;
    int isComment;
    CommDirection direction;
};
typedef struct GetComm GetComm;

GetComm getcomm(char *fline);

#endif