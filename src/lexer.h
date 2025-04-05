#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include "../header/error_code.h"
#include "../header/identifier.h"

void lexer(char *filename);

#endif