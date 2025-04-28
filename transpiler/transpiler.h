#ifndef transpiler_H
#define transpiler_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include "../utils/utils.h"
#include "../flow/flow.h"
#include "../step/step.h"
#include "./enums.h"
#include "./structs.h"
#include "./helper.h"

void transpiler(char *filename);

#endif