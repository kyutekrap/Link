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
    double **data;
} DblList;

typedef struct {
    int count;
    char **data;
} StrList;

typedef struct {
    int count;
    StrList keys;
    StrList values;
} StrMap;

typedef struct {
    int count;
    StrList keys;
    IntList values;
} IntMap;

typedef struct {
    int count;
    StrList keys;
    DblList values;
} DblMap;

void *trim(char *str);
char *substr(char *str, int pos, int cnt);
char *join_str(char *original_str, char *new_str);
IntList append_int_list(IntList int_list, int new_value);
StrList append_str_list(StrList str_list, char *new_value);
YesNo in_str_list(StrList str_list, char *checking_value);
YesNo in_int_list(IntList int_list, int checking_value);
void clear_str_list(StrList *mlist);
void clear_int_list(IntList *mlist);
char *join_str_list(StrList mlist);
char *str_replace(char *str, char from_char, char to_char);
StrMap str_map_set(StrMap str_map, char *key, char *value);
char *str_map_get(StrMap str_map, char *key);
void clear_str_map(StrMap *str_map);
IntMap int_map_set(IntMap map, char *key, int value);
int *int_map_get(IntMap map, char *key);
void clear_int_map(IntMap *map);
StrList str2list(char *str);

#endif