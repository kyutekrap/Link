// ===== EXTERNAL SOURCES (START)

#include "./lib_support.h"

// ===== EXTERNAL SOURCES (END)

// ===== UTILS (START)

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
        case INVALID_filepath:
            return "INVALID_filepath";
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
    }
    return strdup("");
}

void syslogger(char *path, int line, ErrorCode error_code) {
    printf("[Error Path]: %s\n[Error Line]: %i\n[Error Code]: %s\n", path, line, error_code2str(error_code));
    printf("----------------------------------------\n");
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

DataType get_ltype(char *str) {
    DataType ltype = UnknownDataType;
    StrList mlist = str2list(substr(str, 1, strlen(str)-2));

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

// ===== UTILS (END)

// ===== TOP-DOWN PROCEDURAL GROUPING (START)

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

// ===== TOP-DOWN PROCEDURAL GROUPING (END)

// ===== MAIN (START)

void lib_support(char *filepath) {
    FILE *in_file = fopen(filepath, "r");

    TextType text_type = IdentifierText;
    CommentType comment_type = OneLineComment;

    char fline[1024];
    int fline_number = 0;
    while (fgets(fline, sizeof(fline), in_file)) {
        fline_number ++;

        if (is_empty(fline) == Y)
            continue;

        trim(fline);
    
        if (comment_type == OneLineComment) {
            if(is_one_line_comment(fline) == Y)
                continue;
            if (started_multi_line_comment(fline) == Y) {
                comment_type = MultiLineComment;
                continue;
            }
        } else {
            if (ended_multi_line_comment(fline) == Y) {
                comment_type = OneLineComment;
            }
            continue;
        }

        switch(text_type) {
            case IdentifierText:
                summary.identifier_type = find_identifier(fline);
                if (summary.identifier_type != Data) {
                    syslogger(filepath, fline_number, UNKNOWN_IDENTIFIER);
                    goto cleanup;
                }
                text_type = PropertyText;
                break;
            
            case PropertyText:
                if (is_property(fline) == N) {
                    // TODO: Throw Error
                }
                else
                {
                    Property property_obj = parse_property(fline);
                    if (property_obj.property_type != Global) {
                        syslogger(filepath, fline_number, UNKNOWN_PROPERTY);
                        goto cleanup;
                    }

                    StrList mlist = str2list(property_obj.property_value);

                    DataType dtype = get_dtype(mlist.data[1]);
                    if (dtype != Map) {
                        syslogger(filepath, fline_number, UNKNOWN_DATATYPE);
                        goto cleanup;
                    }

                    char *vname = trim(mlist.data[0]);

                    StrList map_args = parse_map(mlist.data[1]);
                    if (map_args.count == 0) {
                        syslogger(filepath, fline_number, UNKNOWN_DATATYPE);
                        goto cleanup;
                    }

                    DataType dtype = get_dtype(map_args.data[0]);
                    if (dtype != List) {
                        syslogger(filepath, fline_number, UNKNOWN_DATATYPE);
                        goto cleanup;
                    }

                    DataType ltype = get_ltype(map_args.data[0]);
                    if (ltype != String) {
                        syslogger(filepath, fline_number, ILLEGAL_DATATYPE);
                        goto cleanup;
                    }

                    StrList elements1 = str2list(substr(map_args.data[0], 1, strlen(map_args.data[0]) - 2));
                    for (int i=0; i<elements1.count; i++) {
                        for (int j=0; j<elements1.count; j++) {
                            if (i == j) continue;
                            if (strcmp(elements1.data[i], elements1.data[j]) == 0) {
                                syslogger(filepath, fline_number, REPEATED_KEY);
                                goto cleanup;
                            }
                        }
                    }

                    dtype = get_dtype(map_args.data[1]);
                    if (dtype != List) {
                        syslogger(filepath, fline_number, UNKNOWN_DATATYPE);
                        goto cleanup;
                    }

                    ltype = get_ltype(map_args.data[1]);
                    if (ltype != String) {
                        syslogger(filepath, fline_number, ILLEGAL_DATATYPE);
                        goto cleanup;
                    }

                    StrList elements2 = str2list(substr(map_args.data[1], 1, strlen(map_args.data[1]) - 2));
                }
                break;
        }
    }

    // TODO: Make file
    // TODO: Delete old file

    cleanup:
        fclose(in_file);
        fclose(out_file);    

    return summary;
}

// ===== MAIN (END)