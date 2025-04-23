#ifndef helper_H
#define helper_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../debugger/debugger.h"
#include "../utils/utils.h"
#include "../flow/flow.h"
#include "../step/step.h"
#include "./enums.h"
#include "./structs.h"

char *error_code2str(ErrorCode error_code);
void syslogger(char *path, int line, ErrorCode error_code);
void *get_full_path(char *cwd, char *relative_path, char *out_path);
IntList char_index(char *str, char target);
IntList char_index_ignore(char *str, char target);
YesNo is_empty(char *fline);
char *extract_string(char *fline);
YesNo is_integer(char *var);
YesNo is_double(char *var);
DataType get_ltype(char *str);
Comparator parse_comparator(char *str);
ComparatorDataType get_comparator_type(char *str, IntMap params);
StrList parse_map(char *map);
YesNo is_one_line_comment(char *fline);
YesNo started_multi_line_comment(char *fline);
YesNo ended_multi_line_comment(char *fline);
IdentifierType find_identifier(char *fline);
YesNo is_property(char *fline);
Property parse_property(char *fline);
DataType get_dtype(char *str);
char *get_cwd(char *filename);

#endif