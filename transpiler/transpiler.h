#ifndef transpiler_H
#define transpiler_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include "./error_code.h"
#include "./identifier.h"
#include "../debugger/debugger.h"

void transpiler(char *filename);

#endif