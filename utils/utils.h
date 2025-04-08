#ifndef utils_H
#define utils_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef enum {
    Y,
    N
} YesNo;

typedef struct {
    int count;
    int **data;
} IntList;

typedef struct {
    int count;
    char **data;
} StrList;

void *trim(char *str);
char *substr(char *str, int pos, int cnt);
char *join_str(char *original_str, char *new_str);
IntList append_int_list(IntList int_list, int new_value);
StrList append_str_list(StrList str_list, char *new_value);
YesNo in_str_list(StrList str_list, char *checking_value);
YesNo in_int_list(IntList int_list, int checking_value);
StrList clear_str_list(StrList mlist);
IntList clear_int_list(IntList mlist);

#endif