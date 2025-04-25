#include "./helper.h"

char *error_code2str(ErrorCode error_code) {
    switch (error_code) {
        case INVALID_FILE:
            return "INVALID_FILE";
        case FILE_NOT_FOUND:
            return "FILE_NOT_FOUND";
        case UNKNOWN_IDENTIFIER:
            return "UNKNOWN_IDENTIFIER";
        case UNKNOWN_PROPERTY:
            return "UNKNOWN_PROPERTY";
        case UNKNOWN_FUNCTION:
            return "UNKNOWN_FUNCTION";
        case C_COMPILE_ERROR:
            return "C_COMPILE_ERROR";
        case UNKNOWN_DATATYPE:
            return "UNKNOWN_DATATYPE";
        case INVALID_FILENAME:
            return "INVALID_FILENAME";
        case PREDEFINED_VARIABLE:
            return "PREDEFINED_VARIABLE";
        case INVALID_IMPORT:
            return "INVALID_IMPORT";
        case UNDEFINED_VARIABLE:
            return "UNDEFINED_VARIABLE";
        case INVALID_VARIABLE:
            return "INVALID_VARIABLE";
        case ILLEGAL_DATATYPE:
            return "ILLEGAL_DATATYPE";
        case REPEATED_KEY:
            return "REPEATED_KEY";
        case UNMATCHED_ELEMENTS:
            return "UNMATCHED_ELEMENTS";
    }
    return strdup("");
}

void syslogger(char *path, int line, ErrorCode error_code) {
    printf("[Error Path]: %s\n[Error Line]: %i\n[Error Code]: %s\n", path, line, error_code2str(error_code));
    printf("----------------------------------------\n");
}

char *get_cwd(char *filename) {
    IntList mlist = char_index(filename, '\\');
    if (mlist.count == 0) {
        return NULL;
    }

    int cnt = *(mlist.data[mlist.count - 1]);
    char *result = substr(filename, 0, cnt);

    for (int i = 0; i < mlist.count; i++) {
        free(mlist.data[i]);
    }
    free(mlist.data);

    return result;
}

void *get_full_path(char *cwd, char *relative_path, char *out_path) {
    char combined_path[PATH_MAX];
    snprintf(combined_path, sizeof(combined_path), "%s%s", cwd, relative_path);
    _fullpath(out_path, combined_path, PATH_MAX);
}

IntList char_index(char *str, char target) {
    IntList mlist = {0, NULL};

    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == target) {
            int **temp = realloc(mlist.data, (mlist.count + 1) * sizeof(int *));
            if (!temp) {
                for (int j = 0; j < mlist.count; j++) {
                    free(mlist.data[j]);
                }
                free(mlist.data);
                mlist.data = NULL;
                mlist.count = 0;
                return mlist;
            }

            mlist.data = temp;

            mlist.data[mlist.count] = malloc(sizeof(int));
            if (!mlist.data[mlist.count]) {
                for (int j = 0; j < mlist.count; j++) {
                    free(mlist.data[j]);
                }
                free(mlist.data);
                mlist.data = NULL;
                mlist.count = 0;
                return mlist;
            }

            *(mlist.data[mlist.count]) = i;
            mlist.count++;
        }
    }

    return mlist;
}

IntList char_index_ignore(char *str, char target) {
    IntList mlist = {0, NULL};
    int inside_brackets = 0;

    for (int i = 0; str[i]; i++) {
        if (str[i] == '[') {
            inside_brackets++;
        } else if (str[i] == ']') {
            if (inside_brackets > 0) inside_brackets--;
        }

        if (str[i] == target && inside_brackets == 0) {
            int **temp = realloc(mlist.data, (mlist.count + 1) * sizeof(int *));
            if (!temp) goto cleanup;

            mlist.data = temp;
            mlist.data[mlist.count] = malloc(sizeof(int));
            if (!mlist.data[mlist.count]) goto cleanup;

            *(mlist.data[mlist.count]) = i;
            mlist.count++;
        }
    }

    return mlist;

cleanup:
    for (int j = 0; j < mlist.count; j++) {
        free(mlist.data[j]);
    }
    free(mlist.data);
    mlist.data = NULL;
    mlist.count = 0;
    return mlist;
}

YesNo is_empty(char *fline) {
    while (*fline) {
        if (!isspace((unsigned char)*fline))
            return N;
        fline++;  
    }
    return Y;
}

char *extract_string(char *fline) {
    return substr(fline, 1, strlen(fline)-2);
}

YesNo is_integer(char *var) {
    if (var == NULL || *var == '\0')
        return N;

    for (int i = 0; var[i] != '\0'; i++) {
        if (!isdigit((unsigned char)var[i])) {
            if (!(var[i] == '-' && i == 0))
                return N;
        }   
    }

    return Y;
}

YesNo is_double(char *var) {
    if (var == NULL || *var == '\0')
        return N;

    int has_point = 0;
    for (int i=0; var[i] != '\0'; i++) {
        if (var[i] == '.') {
            if (has_point == 0)
                has_point = 1;
            else
                return N;
        } else if (!isdigit((unsigned char)var[i])) {
            if (!(var[i] == '-' && i == 0))
                return N;
        }
    }

    return Y;
}

DataType get_ltype(char *str) {
    DataType ltype = UnknownDataType;

    StrList mlist = str2list(str);

    char *current;
    int current_size;
    for (int i=0; i<mlist.count; i++) {
        current = mlist.data[i];
        current_size = strlen(current);
        if (current_size >= 2 && current[0] == '"' && current[current_size-1] == '"') {
            if (ltype = UnknownDataType) {
                ltype = String;
            } else if (ltype != String) {
                ltype = UnknownDataType;
                break;
            }
        } else if (is_integer(current) == Y) {
            if (ltype = UnknownDataType) {
                ltype = Integer;
            } else if (ltype != Integer) {
                ltype = UnknownDataType;
                break;
            }
        } else if (is_double(current) == Y) {
            if (ltype = UnknownDataType) {
                ltype = Double;
            } else if (ltype != Double) {
                ltype = UnknownDataType;
            }
        }
    }

    return ltype;
}

Comparator parse_comparator(char *str) {
    Comparator comparator = {UnknownComparator, NULL};

    int str_len = strlen(str);
    if (is_empty(str) == Y || str_len < 5)
        return comparator;

    IntList mlist = char_index(str, '(');
    if (mlist.count != 1)
        return comparator;

    char *type = substr(str, 0, *mlist.data[0]);
    if (strcmp(type, "eq") == 0) comparator.type = EqualTo;
    else if (strcmp(type, "gt") == 0) comparator.type = GreaterThan;
    else if (strcmp(type, "lt") == 0) comparator.type = LesserThan;
    else if (strcmp(type, "gte") == 0) comparator.type = GreaterOrEqual;
    else if (strcmp(type, "lte") == 0) comparator.type = LesserOrEqual;
    else return comparator;

    comparator.value = substr(str, *mlist.data[0] + 1, str_len - *mlist.data[0] - 2);
    return comparator;
}

ComparatorDataType get_comparator_type(char *str, IntMap params) {
    int str_len = strlen(str);
    if (str_len >= 2 && str[0] == '"' && str[str_len-1] == '"') {
        return StringLiteral;
    }
    else if (is_integer(str) == Y) {
        return IntegerStatic;
    }
    else if (is_double(str) == Y) {
        return DoubleStatic;
    }
    else {
        int *temp_type = int_map_get(params, str);
        if (temp_type == NULL)
            return UnknownDataType;
        else if (*temp_type == String)
            return StringVar;
        else if (*temp_type == Integer)
            return IntegerVar;
        else if (*temp_type == Double)
            return DoubleVar;
    }

    return UnknownComparatorDataType;
}

StrList parse_map(char *map) {
    StrList res = {0, NULL};

    IntList mlist = char_index_ignore(map, ',');
    if (mlist.count != 1)
        return res;

    char *res1 = trim(substr(map, 1, *mlist.data[0] - 1));
    char *res2 = trim(substr(map, *mlist.data[0] + 1, strlen(map) - *mlist.data[0] - 2));

    if (is_empty(res1) == Y || is_empty(res2) == Y)
        return res;

    res.count = 2;
    res.data = malloc(2 * sizeof(char*));
    res.data[0] = res1;
    res.data[1] = res2;

    return res;
}

YesNo is_one_line_comment(char *fline) {
    const int pos = 0;
    const int cnt = 2;
    if (strcmp(substr(fline, pos, cnt), "//") == 0)
        return Y;
    else
        return N;
}

YesNo started_multi_line_comment(char *fline) {
    const int pos = 0;
    const int cnt = 2;
    if (strcmp(substr(fline, pos, cnt), "/*") == 0)
        return Y;
    else
        return N;
}

YesNo ended_multi_line_comment(char *fline) {
    const int pos = strlen(fline) - 2;
    const int cnt = 2;
    if (strcmp(substr(fline, pos, cnt), "*/") == 0)
        return Y;
    else
        return N;
}

IdentifierType find_identifier(char *fline) {
    if (strcmp(fline, "#flow") == 0)
        return Flow;
    else if (strcmp(fline, "#step") == 0)
        return Step;
    else if (strcmp(fline, "#data") == 0)
        return Data;
    else
        return UnknownIdentifier;
}

YesNo is_property(char *fline) {
    if (fline[0] == '@')
        return Y;
    else
        return N;
}

Property parse_property(char *fline) {
    Property property_obj = {UnknownProperty, NULL};

    IntList sValue = char_index(fline, '(');
    if (sValue.count == 0) {
        return property_obj;
    }

    int offset = strlen(fline) - 1;
    if (fline[offset] != ')') {
        clear_int_list(sValue);
        return property_obj;
    }

    char *property_type = substr(fline, 1, sValue.data[0][0] - 1);
    if (strcmp(property_type, "debug") == 0)
        property_obj.property_type = Debug;
    else if (strcmp(property_type, "import") == 0)
        property_obj.property_type = Import;
    else if (strcmp(property_type, "global") == 0)
        property_obj.property_type = Global;
    else {
        free(property_type);
        clear_int_list(sValue);
        return property_obj;
    }
    free(property_type);

    int pos = sValue.data[0][0] + 1;
    int cnt = offset - pos;
    property_obj.property_value = substr(fline, pos, cnt);
    clear_int_list(sValue);

    return property_obj;
}

DataType get_dtype(char *str) {
    int str_len = strlen(str);
    if (str_len == 0)
    {
        return None;
    }
    else if (is_integer(str) == Y)
    {
        return Integer;
    }
    else if (is_double(str) == Y)
    {
        return Double;
    }
    else if (str_len >= 2 && str[0] == '"' && str[str_len-1] == '"')
    {
        return String;
    }
    else if (str_len >= 2 && str[0] == '[' && str[str_len-1] == ']')
    {
        return List;
    }
    else if (str_len >= 7 && str[0] == '{' && str[str_len-1] == '}')
    {
        return Map;
    }

    return UnknownDataType;
}