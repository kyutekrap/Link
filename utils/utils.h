#ifndef utils_H
#define utils_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

void *trim(char *str);
char *substr(char *str, int pos, int cnt);
char *join_str(char *original_str, const char *new_str);

#endif