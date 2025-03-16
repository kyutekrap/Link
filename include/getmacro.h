#ifndef MACRO_H
#define MACRO_H

#include <ctype.h>
#include <string.h>

enum MacroT {
    flow,
    step,
    invalid
};
typedef enum MacroT MacroT;

MacroT getmacro(const char *fline);

#endif