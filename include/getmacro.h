#ifndef MACRO_H
#define MACRO_H

#include <ctype.h>
#include <string.h>
#include "../macro/errcode.h"

enum MacroT {
    flow,
    step,
    invalid
};
typedef enum MacroT MacroT;

struct GetMacro {
    MacroT macroT;
    int errCode;
};
typedef struct GetMacro GetMacro;

GetMacro getmacro(const char *fline);

#endif