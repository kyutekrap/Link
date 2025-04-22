#ifndef lib_support_H
#define lib_support_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "./error_code.h"
#include "./enums.h"
#include "./structs.h"

void lib_support(char *filepath);

#endif