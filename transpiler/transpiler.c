// ===== EXTERNAL SOURCES (START)

#include "./transpiler.h"

// ===== EXTERNAL SOURCES (END)

// ===== DEFINITIONS (START)

#define HEADER "#include <string.h>\n"
#define HEADER_DEBUG "#include <stdio.h>\n#include <time.h>\n\n"

// ===== DEFINITIONS (END)

// ===== TOP-DOWN PROCEDURAL GROUPING (START)

YesNo is_link_file(char *filename) {
    const int cnt = 5;
    const int pos = strlen(filename) - cnt;
    if (pos < 1)
        return N;
    char *suffix = substr(filename, pos, cnt);
    YesNo result = (suffix && strcmp(suffix, ".link") == 0) ? Y : N;
    free(suffix);
    return result;
}

char *get_namespace(char *filename, char *cwd) {
    char *result;
    int pos = strlen(cwd) + 1;
    IntList mlist = char_index(filename, '.');
    int cnt = mlist.data[0][0] - pos;
    result = substr(filename, pos, cnt);
    return result;
}

YesNo is_valid_filename(char *filename) {
    if (isalpha(filename[0]) || filename[0] == '_')
        return Y;
    else
        return N;
}

char *get_target_name(char *filename) {
    int len = strlen(filename);
    char *new_str = malloc(len - 3);
    if (!new_str) return NULL;
    strncpy(new_str, filename, len - 4);
    new_str[len - 4] = 'c';
    new_str[len - 3] = '\0';
    return new_str;
}

StrList search_files(char *cwd, char *extension) {
    StrList mlist = {0, NULL};
    struct dirent *dp;
    DIR *dir = opendir(cwd);

    if (!dir) {
        printf("[Error] Failed to open directory: %s\n", cwd);
        return mlist;
    }

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;

        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s\\\\%s", cwd, dp->d_name);

        struct stat path_stat;
        stat(path, &path_stat);

        if (S_ISREG(path_stat.st_mode)) {
            const char *ext = strrchr(dp->d_name, '.');
            if (ext && strcmp(ext, extension) == 0) {
                char full_path[PATH_MAX];
                char temp_path[PATH_MAX];

                snprintf(temp_path, sizeof(temp_path), "%s%s%s",
                        cwd,
                        (cwd[strlen(cwd) - 1] == '\\') ? "" : "\\",
                        dp->d_name);

                get_full_path("", temp_path, full_path);

                mlist.count++;
                mlist.data = realloc(mlist.data, mlist.count * sizeof(char *));
                if (!mlist.data) {
                    printf("[Error] Memory allocation failed\n");
                    closedir(dir);
                    mlist.count = 0;
                    return mlist;
                }

                mlist.data[mlist.count - 1] = strdup(full_path);
            }
        }
    }

    closedir(dir);
    return mlist;
}

StrList search_dirs(char *cwd) {
    StrList mlist = {0, NULL};
    struct dirent *dp;
    DIR *dir = opendir(cwd);

    if (!dir) {
        printf("[Error] Failed to open directory: %s\n", cwd);
        return mlist;
    }

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;

        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s\\%s", cwd, dp->d_name);

        struct stat path_stat;
        stat(path, &path_stat);

        if (S_ISDIR(path_stat.st_mode)) {
            char full_path[PATH_MAX];
            char temp_path[PATH_MAX];

            snprintf(temp_path, sizeof(temp_path), "%s%s%s",
                    cwd,
                    (cwd[strlen(cwd) - 1] == '\\') ? "" : "\\",
                    dp->d_name);

            get_full_path("", temp_path, full_path);

            mlist.count++;
            mlist.data = realloc(mlist.data, mlist.count * sizeof(char *));
            if (!mlist.data) {
                printf("[Error] Memory allocation failed\n");
                closedir(dir);
                mlist.count = 0;
                return mlist;
            }

            mlist.data[mlist.count - 1] = strdup(full_path);
        }
    }

    closedir(dir);
    return mlist;
}

char delete_old_files(char *cwd) {
    StrList files = search_files(cwd, ".c");
    for (int i = 0; i < files.count; i ++) {
        remove(files.data[i]);
    }
    StrList dirs = search_dirs(cwd);
    for (int i = 0; i < dirs.count; i ++) {
        StrList files = search_files(dirs.data[i], ".c");
        for (int i = 0; i < files.count; i ++) {
            remove(files.data[i]);
        }
    }
}

void read_and_copy_file(const char *full_path_c, char **import_script) {
    FILE *fp = fopen(full_path_c, "r");
    if (fp == NULL)
        return;

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return;
    }

    long length = ftell(fp);
    if (length < 0) {
        fclose(fp);
        return;
    }

    rewind(fp);

    char *buffer = malloc(length + 3);
    if (!buffer) {
        fclose(fp);
        printf("[Error] Memory allocation failed\n");
        return;
    }

    size_t read_size = fread(buffer, 1, length, fp);
    fclose(fp);

    if (read_size == 0 && ferror(fp)) {
        free(buffer);
        return;
    }

    buffer[read_size] = '\0';

    while (read_size > 0 && (buffer[read_size - 1] == '\n' || buffer[read_size - 1] == '\r')) {
        buffer[--read_size] = '\0';
    }

    buffer[read_size++] = '\n';
    buffer[read_size++] = '\n';
    buffer[read_size] = '\0';

    size_t old_len = (*import_script) ? strlen(*import_script) : 0;
    char *new_str = realloc(*import_script, old_len + read_size + 1);
    if (!new_str) {
        free(buffer);
        printf("[Error] Memory allocation failed\n");
        return;
    }

    memcpy(new_str + old_len, buffer, read_size + 1);
    free(buffer);

    *import_script = new_str;
}

YesNo is_function(char *fline) {
    if (is_valid_filename(fline) == N)
        return N;

    IntList sValue = char_index(fline, '(');
    if (sValue.count == 0)
        return N;
    
    int offset = strlen(fline) - 1;
    if (fline[offset] != ')')
        return N;

    return Y;
}

Function parse_function(char *fline, StrMap imports) {
    Function function = {strdup("UnknownFunction"), NULL};

    int flen = strlen(fline);
    if (fline[flen-1] != ')')
        return function;

    IntList sValue = char_index(fline, '(');
    int pos = sValue.data[0][0]+1;
    int cnt = flen - pos - 1;
    function.function_value = trim(substr(fline, pos, cnt));

    char *temp = substr(fline, 0, sValue.data[0][0]);
    if (strcmp(temp, "info") == 0)
    {
        StrList mlist = str2list(function.function_value);
        if (mlist.count != 1)
            return function;

        function.function_type = "info";
    }
    else if (strcmp(temp, "warning") == 0)
    {
        StrList mlist = str2list(function.function_value);
        if (mlist.count != 1)
            return function;

        function.function_type = "warning";
    }
    else if (strcmp(temp, "error") == 0)
    {
        StrList mlist = str2list(function.function_value);
        if (mlist.count != 1)
            return function;

        function.function_type = "error";
    }
    else if (strcmp(temp, "die") == 0)
    {
        StrList mlist = str2list(function.function_value);
        if (mlist.count != 0)
            return function;

        function.function_type = "die";
    }
    else if (strcmp(temp, "depends") == 0)
    {
        StrList mlist = str2list(function.function_value);
        if (mlist.count != 2)
            return function;

        function.function_type = "depends";
    }
    else if (strcmp(temp, "while") == 0)
    {
        StrList mlist = str2list(function.function_value);
        if (mlist.count != 2)
            return function;

        function.function_type = "while";
    }
    else {
        char *sub_to = str_map_get(imports, temp);
        if (sub_to == NULL)
            return function;

        StrList mlist = str2list(function.function_value);
        if (mlist.count != 0)
            return function;

        function.function_type = sub_to;
    }

    return function;
}

YesNo is_valid_varname(char *varname) {
    int str_len = strlen(varname);
    if (str_len == 0)
        return N;

    if (!(varname[0] == '_' || isalpha(varname[0])))
        return N;

    char last = varname[str_len-1];
    if (!(last == '_' || isalpha(last) || isdigit(last)))
        return N;

    for (int i=0; i<str_len; i++) {
        if (isspace(varname[i]))
            return N;
    }

    return Y;
}

char *print_die(char *namespace, YesNo debug, IdentifierType identifier_type) {
    char *output = NULL;
    char *extra = NULL;

    if (debug == Y) {
        if (identifier_type == Flow) {
            extra = flow_e(namespace);
        } else {
            extra = step_e(namespace);
        }
    }

    const char *return_line = " return;";

    size_t extra_len = extra ? strlen(extra) : 0;
    size_t total_len = extra_len + strlen(return_line) + 1;

    output = malloc(total_len);
    if (!output) return NULL;

    output[0] = '\0';
    if (extra) {
        strcat(output, extra);
    }
    strcat(output, return_line);

    if (extra) free(extra);
    return output;
}

// ===== TOP-DOWN PROCEDURAL GROUPING (END)

// ===== HELPERS (START)

ErrorCode analyze_free_line_text(char *fline, FILE *out_file, YesNo debug, StrMap imports, IdentifierType identifier_type, char *namespace, IntMap params) {
    if (is_function(fline) == N)
        return UNKNOWN_FUNCTION;

    Function function = parse_function(fline, imports);
    if (strcmp(function.function_type, "UnknownFunction") == 0)
    {
        return UNKNOWN_FUNCTION;
    }
    else if (strcmp(function.function_type, "info") == 0)
    {
        if (debug == N)
            return NO_ERROR;

        int func_len = strlen(function.function_value);
        if (!(func_len >= 2 && function.function_value[0] == '"' && function.function_value[func_len-1] == '"')) {
            int *dtype = int_map_get(params, function.function_value);
            if (dtype == NULL || (*dtype != String && *dtype != Integer))
                return UNDEFINED_VARIABLE;
        }

        char *info_temp = info(function.function_value);
        fprintf(out_file, "\t%s", info_temp);
        free(info_temp);
    }
    else if (strcmp(function.function_type, "warning") == 0)
    {
        if (debug == N)
            return NO_ERROR;

        int func_len = strlen(function.function_value);
        if (!(func_len >= 2 && function.function_value[0] == '"' && function.function_value[func_len-1] == '"')) {
            int *dtype = int_map_get(params, function.function_value);
            if (dtype == NULL || (*dtype != String && *dtype != Integer))
                return UNDEFINED_VARIABLE;
        }

        char *warning_temp = warning(function.function_value);
        fprintf(out_file, "\t%s", warning_temp);
        free(warning_temp);
    }
    else if (strcmp(function.function_type, "error") == 0)
    {
        if (debug == N)
            return NO_ERROR;

        int func_len = strlen(function.function_value);
        if (!(func_len >= 2 && function.function_value[0] == '"' && function.function_value[func_len-1] == '"')) {
            int *dtype = int_map_get(params, function.function_value);
            if (dtype == NULL || (*dtype != String && *dtype != Integer))
                return UNDEFINED_VARIABLE;
        }

        char *error_temp = error(function.function_value);
        fprintf(out_file, "\t%s", error_temp);
        free(error_temp);
    }
    else if (strcmp(function.function_type, "die") == 0)
    {
        fputs(print_die(namespace, debug, identifier_type), out_file);
    }
    else if (strcmp(function.function_type, "global") == 0)
    {
        StrList mlist = str2list(function.function_value);
        if (mlist.count != 2)
            return UNKNOWN_FUNCTION;

        if (is_valid_varname(trim(mlist.data[0])) == N)
            return INVALID_VARIABLE;

        DataType dtype = get_dtype(mlist.data[1]);
        if (dtype == UnknownDataType)
            return UNKNOWN_DATATYPE;
    }
    else if (strcmp(function.function_type, "depends") == 0)
    {
        StrList mlist = str2list(function.function_value);
        if (int_map_get(params, mlist.data[0]) == NULL)
            return UNDEFINED_VARIABLE;

        if (get_dtype(mlist.data[1]) != Map)
            return UNKNOWN_FUNCTION;

        StrList map_args = str2list(substr(mlist.data[1], 1, strlen(mlist.data[1])-2));
        if (map_args.count != 2)
            return UNKNOWN_FUNCTION;

        if (!(get_dtype(map_args.data[0]) == List && get_dtype(map_args.data[1]) == List))
            return UNKNOWN_FUNCTION;

        StrList args1 = str2list(substr(map_args.data[0], 1, strlen(map_args.data[0])-2));
        StrList args2 = str2list(substr(map_args.data[1], 1, strlen(map_args.data[1])-2));

        if (args1.count != args2.count)
            return UNKNOWN_FUNCTION;

        Function temp_func;
        Comparator comparator;
        ComparatorDataType comparator_type;
        for (int i=0; i<args1.count; i++) {
            comparator = parse_comparator(args1.data[i]);
            if (comparator.type == UnknownComparator)
                return UNKNOWN_FUNCTION;

            comparator_type = get_comparator_type(comparator.value, params);
            if (comparator_type == UnknownComparatorDataType)
                return UNKNOWN_FUNCTION;

            temp_func = parse_function(args2.data[i], imports);
            if (temp_func.function_type == "UnknownFunction")
                return UNKNOWN_FUNCTION;

            if (strcmp(temp_func.function_type, "die") == 0) {
                char *inside = print_die(namespace, debug, identifier_type);
                size_t total_len = strlen(inside) + strlen("{ }\n") + 1;

                char *wrapped = malloc(total_len);
                if (!wrapped)
                    return C_COMPILE_ERROR;

                sprintf(wrapped, "{ %s }\n", inside);
                free(inside);
                temp_func.function_type = wrapped;
            } else {
                size_t len = strlen(temp_func.function_type) + strlen("();\n") + 1;
                char *new_str = malloc(len);
                if (new_str == NULL)
                    return C_COMPILE_ERROR;
                
                sprintf(new_str, "{ %s(); }\n", temp_func.function_type);
                temp_func.function_type = new_str;
            }

            if (comparator.type == EqualTo) {
                if (comparator_type == String) {
                    if (i == 0) {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\tif (strcmp(%s, strdup(%s)) == 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\tif (strcmp(%s, %s) == 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    } else {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\telse if (strcmp(%s, strdup(%s)) == 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\telse if (strcmp(%s, %s) == 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    }
                }
                else if (comparator_type == Integer || comparator_type == Double) {
                    if (i == 0) {
                        fprintf(out_file, "\tif (%s == %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    } else {
                        fprintf(out_file, "\telse if (%s == %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    }
                }
            }
            else if (comparator.type == GreaterThan) {
                if (comparator_type == String) {
                    if (i == 0) {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\tif (strcmp(%s, strdup(%s)) > 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\tif (strcmp(%s, %s) > 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    } else {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\telse if (strcmp(%s, strdup(%s)) > 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\telse if (strcmp(%s, %s) > 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    }
                }
                else if (comparator_type == Integer || comparator_type == Double) {
                    if (i == 0) {
                        fprintf(out_file, "\tif (%s > %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    } else {
                        fprintf(out_file, "\telse if (%s > %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    }
                }
            }
            else if (comparator.type == LesserThan) {
                if (comparator_type == String) {
                    if (i == 0) {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\tif (strcmp(%s, strdup(%s)) < 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\tif (strcmp(%s, %s) < 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    } else {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\telse if (strcmp(%s, strdup(%s)) < 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\telse if (strcmp(%s, %s) < 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    }
                }
                else if (comparator_type == Integer || comparator_type == Double) {
                    if (i == 0) {
                        fprintf(out_file, "\tif (%s < %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    } else {
                        fprintf(out_file, "\telse if (%s < %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    }
                }
            }
            else if (comparator.type == GreaterOrEqual) {
                if (comparator_type == String) {
                    if (i == 0) {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\tif (strcmp(%s, strdup(%s)) >= 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\tif (strcmp(%s, %s) >= 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    } else {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\telse if (strcmp(%s, strdup(%s)) >= 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\telse if (strcmp(%s, %s) >= 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    }
                }
                else if (comparator_type == Integer || comparator_type == Double) {
                    if (i == 0) {
                        fprintf(out_file, "\tif (%s => %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    } else {
                        fprintf(out_file, "\telse if (%s => %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    }
                }
            }
            else if (comparator.type == LesserOrEqual) {
                if (comparator_type == String) {
                    if (i == 0) {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\tif (strcmp(%s, strdup(%s)) <= 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\tif (strcmp(%s, %s) <= 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    } else {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\telse if (strcmp(%s, strdup(%s)) <= 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\telse if (strcmp(%s, %s) <= 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    }
                }
                else if (comparator_type == Integer || comparator_type == Double) {
                    if (i == 0) {
                        fprintf(out_file, "\tif (%s <= %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    } else {
                        fprintf(out_file, "\telse if (%s <= %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    }
                }
            }
        }
    }
    else if (strcmp(function.function_type, "while") == 0)
    {
        StrList mlist = str2list(function.function_value);
        if (int_map_get(params, mlist.data[0]) == NULL)
            return UNDEFINED_VARIABLE;

        if (get_dtype(mlist.data[1]) != Map)
            return UNKNOWN_FUNCTION;

        StrList map_args = str2list(substr(mlist.data[1], 1, strlen(mlist.data[1])-2));
        if (map_args.count != 2)
            return UNKNOWN_FUNCTION;

        if (!(get_dtype(map_args.data[0]) == List && get_dtype(map_args.data[1]) == List))
            return UNKNOWN_FUNCTION;

        StrList args1 = str2list(substr(map_args.data[0], 1, strlen(map_args.data[0])-2));
        StrList args2 = str2list(substr(map_args.data[1], 1, strlen(map_args.data[1])-2));

        if (args1.count != args2.count)
            return UNKNOWN_FUNCTION;

        Function temp_func;
        Comparator comparator;
        ComparatorDataType comparator_type;
        for (int i=0; i<args1.count; i++) {
            comparator = parse_comparator(args1.data[i]);
            if (comparator.type == UnknownComparator)
                return UNKNOWN_FUNCTION;

            comparator_type = get_comparator_type(comparator.value, params);
            if (comparator_type == UnknownComparatorDataType)
                return UNKNOWN_FUNCTION;

            temp_func = parse_function(args2.data[i], imports);
            if (temp_func.function_type == "UnknownFunction")
                return UNKNOWN_FUNCTION;

            if (strcmp(temp_func.function_type, "die") == 0) {
                char *inside = print_die(namespace, debug, identifier_type);
                size_t total_len = strlen(inside) + strlen("{ }\n") + 1;

                char *wrapped = malloc(total_len);
                if (!wrapped)
                    return C_COMPILE_ERROR;

                if (i == 0) sprintf(wrapped, "{\n\t\t%s\n", inside);
                else sprintf(wrapped, "{ %s }\n", inside);
                free(inside);
                temp_func.function_type = wrapped;
            } else {
                size_t len = strlen(temp_func.function_type) + strlen("();\n") + 1;
                char *new_str = malloc(len);
                if (new_str == NULL)
                    return C_COMPILE_ERROR;
                
                if (i == 0) sprintf(new_str, "{\n\t\t%s();\n", temp_func.function_type);
                else sprintf(new_str, "{ %s(); }\n", temp_func.function_type);
                temp_func.function_type = new_str;
            }

            if (comparator.type == EqualTo) {
                if (comparator_type == String) {
                    if (i == 0) {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\twhile (strcmp(%s, strdup(%s)) == 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\twhile (strcmp(%s, %s) == 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    } else {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\t\tif (strcmp(%s, strdup(%s)) == 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\t\tif (strcmp(%s, %s) == 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    }
                }
                else if (comparator_type == Integer || comparator_type == Double) {
                    if (i == 0) {
                        fprintf(out_file, "\twhile (%s == %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    } else {
                        fprintf(out_file, "\t\tif (%s == %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    }
                }
            }
            else if (comparator.type == GreaterThan) {
                if (comparator_type == String) {
                    if (i == 0) {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\twhile (strcmp(%s, strdup(%s)) > 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\twhile (strcmp(%s, %s) > 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    } else {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\t\tif (strcmp(%s, strdup(%s)) > 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\t\tif (strcmp(%s, %s) > 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    }
                }
                else if (comparator_type == Integer || comparator_type == Double) {
                    if (i == 0) {
                        fprintf(out_file, "\twhile (%s > %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    } else {
                        fprintf(out_file, "\t\tif (%s > %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    }
                }
            }
            else if (comparator.type == LesserThan) {
                if (comparator_type == String) {
                    if (i == 0) {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\twhile (strcmp(%s, strdup(%s)) < 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\twhile (strcmp(%s, %s) < 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    } else {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\t\tif (strcmp(%s, strdup(%s)) < 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\t\tif (strcmp(%s, %s) < 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    }
                }
                else if (comparator_type == Integer || comparator_type == Double) {
                    if (i == 0) {
                        fprintf(out_file, "\twhile (%s < %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    } else {
                        fprintf(out_file, "\t\tif (%s < %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    }
                }
            }
            else if (comparator.type == GreaterOrEqual) {
                if (comparator_type == String) {
                    if (i == 0) {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\twhile (strcmp(%s, strdup(%s)) >= 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\twhile (strcmp(%s, %s) >= 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    } else {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\t\tif (strcmp(%s, strdup(%s)) >= 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\t\tif (strcmp(%s, %s) >= 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    }
                }
                else if (comparator_type == Integer || comparator_type == Double) {
                    if (i == 0) {
                        fprintf(out_file, "\twhile (%s => %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    } else {
                        fprintf(out_file, "\t\tif (%s => %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    }
                }
            }
            else if (comparator.type == LesserOrEqual) {
                if (comparator_type == String) {
                    if (i == 0) {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\twhile (strcmp(%s, strdup(%s)) <= 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\twhile (strcmp(%s, %s) <= 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    } else {
                        if (comparator_type == StringLiteral) {
                            fprintf(out_file, "\t\tif (strcmp(%s, strdup(%s)) <= 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        } else if (comparator_type == StringVar) {
                            fprintf(out_file, "\t\tif (strcmp(%s, %s) <= 0) %s", mlist.data[0], comparator.value, temp_func.function_type);
                        }
                    }
                }
                else if (comparator_type == Integer || comparator_type == Double) {
                    if (i == 0) {
                        fprintf(out_file, "\twhile (%s <= %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    } else {
                        fprintf(out_file, "\t\tif (%s <= %s) %s", mlist.data[0], comparator.value, temp_func.function_type);
                    }
                }
            }
        }
        fputs("\t}\n", out_file);
    }
    else
    {
        fprintf(out_file, "\t%s(%s);\n", function.function_type, function.function_value);
    }

    return NO_ERROR;
}

// ===== HELPERS (END)

// ===== MAIN (START)

TranspilerSummary transpiler_main(char *filename, char *origin, YesNo debug, IntMap *params, IntMap *paramSize) {
    TranspilerSummary summary = {NULL, {0, NULL}, debug, Flow};

    char *namespace = get_namespace(filename, origin);
    summary.name = str_replace(namespace, '\\', '_');
    if (is_valid_filename(summary.name) == N) {
        syslogger(filename, 0, INVALID_FILENAME);
        return summary;
    }
    
    if (is_link_file(filename) == N) {
        syslogger(filename, 0, INVALID_FILE);
        return summary;
    }

    FILE *in_file = fopen(filename, "r");
    if (in_file == NULL) {
        syslogger(filename, 0, FILE_NOT_FOUND);
        return summary;
    }

    char *target_name = get_target_name(filename);
    FILE *out_file = fopen(target_name, "w");
    if (out_file == NULL) {
        fclose(in_file);
        syslogger(filename, 0, C_COMPILE_ERROR);
        return summary;
    }
    
    TextType text_type = IdentifierText;
    CommentType comment_type = OneLineComment;
    char *header_script = NULL;
    char *import_script = NULL;
    char *global_script = NULL;
    StrMap import_dict = {0, {0, NULL}, {0, NULL}};

    char fline[1024];
    int fline_number = 0;
    while (fgets(fline, sizeof(fline), in_file)) {
        fline_number ++;

        if (is_empty(fline) == Y)
            continue;

        trim(fline);

        if (!(isalpha(fline[0]) || fline[0] != '_')) {
            syslogger(filename, fline_number, UNKNOWN_FUNCTION);
            goto cleanup;
        }
    
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
                if (summary.identifier_type == UnknownIdentifier) {
                    syslogger(filename, fline_number, UNKNOWN_IDENTIFIER);
                    goto cleanup;
                }
                text_type = PropertyText;
                break;
            
            case PropertyText:
                if (is_property(fline) == N) {
                    if (summary.identifier_type == Data) {
                        syslogger(filename, fline_number, UNKNOWN_PROPERTY);
                        goto cleanup;
                    }
                    if (header_script != NULL)
                        fputs(header_script, out_file);
                    if (summary.identifier_type == Flow)
                        fputs(HEADER, out_file);
                    if (summary.debug == Y && summary.identifier_type == Flow)
                        fputs(HEADER_DEBUG, out_file);
                    if (global_script != NULL) {
                        fputs(global_script, out_file);
                        free(global_script);
                        fputs("\n", out_file);
                    }
                    if (import_script != NULL) {
                        fwrite(import_script, 1, strlen(import_script), out_file);
                        free(import_script);
                    }
                    fprintf(out_file, "void %s() {\n", summary.name);
                    if (summary.debug == Y) {
                        if (summary.identifier_type == Flow) {
                            char *flow_s_temp = flow_s();
                            fputs(flow_s_temp, out_file);
                            free(flow_s_temp);
                        } else {
                            char *step_s_temp = step_s();
                            fputs(step_s_temp, out_file);
                            free(step_s_temp);
                        }
                    } else {
                        fputs("\n", out_file);
                    }
                    ErrorCode err_cd = analyze_free_line_text(fline, out_file, summary.debug, import_dict, summary.identifier_type, summary.name, *params);
                    if (err_cd != NO_ERROR && err_cd != UNKNOWN_FUNCTION) {
                        syslogger(filename, fline_number, err_cd);
                        goto cleanup;
                    }
                    text_type = FreeLineText;
                    break;
                }
                Property property_obj = parse_property(fline);
                if (summary.identifier_type == Data && property_obj.property_type != Global) {
                    syslogger(filename, fline_number, UNKNOWN_PROPERTY);
                    goto cleanup;
                }
                if (property_obj.property_type == Debug) {
                    summary.debug = (strcmp(property_obj.property_value, "true") == 0) ? Y : N;
                }
                else if (property_obj.property_type == Import) {
                    property_obj.property_value = extract_string(property_obj.property_value);
                    if (is_link_file(property_obj.property_value) == Y)
                    {
                        char *cwd = get_cwd(filename);
                        char full_path[PATH_MAX];
                        get_full_path(cwd, property_obj.property_value, full_path);
                        if (full_path == NULL) {
                            syslogger(filename, fline_number, FILE_NOT_FOUND);
                            goto cleanup;
                        } else {
                            char *func_namespace = get_namespace(full_path, origin);
                            if (is_valid_filename(func_namespace) == N) {
                                syslogger(filename, fline_number, INVALID_FILENAME);
                                goto cleanup;
                            }
    
                            if (in_str_list(summary.imports, func_namespace) == Y) {
                                free(cwd);
                                free(func_namespace);
                                break;
                            }
    
                            char *full_path_c = get_target_name(full_path);
                            if (access(full_path_c, F_OK) != 0) {
                                TranspilerSummary func_summary = transpiler_main(full_path, origin, summary.debug, params, paramSize);
                                if (summary.identifier_type == Flow && func_summary.identifier_type == Flow) {
                                    free(cwd);
                                    free(func_namespace);
                                    syslogger(filename, fline_number, INVALID_IMPORT);
                                    goto cleanup;
                                }
                                else if (summary.identifier_type == Step && func_summary.identifier_type != Step) {
                                    free(cwd);
                                    free(func_namespace);
                                    syslogger(filename, fline_number, INVALID_IMPORT);
                                    goto cleanup;
                                }
                                if (in_str_list(func_summary.imports, func_namespace) == Y) {
                                    free(cwd);
                                    free(func_namespace);
                                    break;
                                }
                            }
                                
                            read_and_copy_file(full_path_c, &import_script);
                            summary.imports = append_str_list(summary.imports, func_namespace);
                            
                            char *func_cwd = get_cwd(full_path);
                            import_dict = str_map_set(import_dict, get_namespace(full_path, func_cwd), str_replace(func_namespace, '\\', '_'));
                        }
                        free(cwd);
                    }
                    else
                    {
                        char *cwd = get_cwd(filename);
                        char full_path[PATH_MAX];
                        get_full_path(cwd, property_obj.property_value, full_path);
                        if (full_path == NULL) {
                            syslogger(filename, fline_number, INVALID_FILE);
                            goto cleanup;
                        }
                        if (access(full_path, F_OK) != 0) {
                            syslogger(filename, fline_number, FILE_NOT_FOUND);
                            goto cleanup;
                        }

                        size_t str_len = strlen(property_obj.property_value) + 1;
                        int line1_len = snprintf(NULL, 0, "#include \"%s\";\n", property_obj.property_value);
                        char *buffer1 = malloc(line1_len + 1);
                        sprintf(buffer1, "#include \"%s\";\n", property_obj.property_value);
                        header_script = join_str(header_script, buffer1);
                        free(buffer1);

                        free(cwd);
                    }
                }
                else if (property_obj.property_type == Global) {
                    if (summary.identifier_type == Step) {
                        syslogger(filename, fline_number, UNKNOWN_PROPERTY);
                        goto cleanup;
                    }

                    StrList mlist = str2list(property_obj.property_value);

                    DataType dtype = get_dtype(mlist.data[1]);
                    if (dtype == String)
                    {
                        char *vname = trim(mlist.data[0]);
                        if (int_map_get(*params, vname) != NULL) {
                            syslogger(filename, fline_number, PREDEFINED_VARIABLE);
                            goto cleanup;
                        }

                        int line_len = snprintf(NULL, 0, "const char %s[] = %s;\n", vname, mlist.data[1]);
                        char *buffer = malloc(line_len + 1);
                        if (!buffer) {
                            syslogger(filename, fline_number, C_COMPILE_ERROR);
                            goto cleanup;
                        }
                        sprintf(buffer, "const char %s[] = %s;\n", vname, mlist.data[1]);
                        global_script = join_str(global_script, buffer);
                        free(buffer);

                        *params = int_map_set(*params, vname, String);
                        free(vname);
                    }
                    else if (dtype == Integer)
                    {
                        char *vname = trim(mlist.data[0]);
                        if (int_map_get(*params, vname) != NULL) {
                            syslogger(filename, fline_number, PREDEFINED_VARIABLE);
                            goto cleanup;
                        }

                        int line1_len = snprintf(NULL, 0, "const int %s = %s;\n", vname, mlist.data[1]);
                        char *buffer1 = malloc(line1_len + 1);
                        sprintf(buffer1, "const int %s = %s;\n", vname, mlist.data[1]);
                        global_script = join_str(global_script, buffer1);
                        free(buffer1);

                        *params = int_map_set(*params, vname, Integer);
                        free(vname);
                    }
                    else if (dtype == Double)
                    {
                        char *vname = trim(mlist.data[0]);
                        if (int_map_get(*params, vname) != NULL) {
                            syslogger(filename, fline_number, PREDEFINED_VARIABLE);
                            goto cleanup;
                        }

                        int line1_len = snprintf(NULL, 0, "const double %s = %s;\n", vname, mlist.data[1]);
                        char *buffer1 = malloc(line1_len + 1);
                        sprintf(buffer1, "const double %s = %s;\n", vname, mlist.data[1]);
                        global_script = join_str(global_script, buffer1);
                        free(buffer1);

                        *params = int_map_set(*params, vname, Double);
                        free(vname);
                    }
                    else if (dtype == List)
                    {
                        char *vname = trim(mlist.data[0]);
                        if (int_map_get(*params, vname) != NULL) {
                            syslogger(filename, fline_number, PREDEFINED_VARIABLE);
                            goto cleanup;
                        }

                        char *data_trimmed = trim(mlist.data[1]);
                        char *data = substr(data_trimmed, 1, strlen(data_trimmed)-2);
                        IntList tokens = char_index_ignore(data, ',');
                        if (tokens.count == 0) {
                            free(vname);
                            syslogger(filename, fline_number, UNKNOWN_DATATYPE);
                            goto cleanup;
                        }

                        YesNo is_grid = N;
                        DataType type = Integer;
                        int count;
                        for (int i=0; i<=tokens.count; i++) {
                            if (i == 0) {
                                char *token = trim(substr(data, i, *tokens.data[i]));
                                if (is_integer(token) == Y) {
                                    int line1_len = snprintf(NULL, 0, "const int %s[%d] = {%s", vname, tokens.count, token);
                                    char *buffer1 = malloc(line1_len + 1);
                                    sprintf(buffer1, "const int %s[%d] = {%s", vname, tokens.count, token);
                                    global_script = join_str(global_script, buffer1);
                                    free(buffer1);
                                    *params = int_map_set(*params, vname, IntegerList);
                                    *paramSize = int_map_set(*params, vname, tokens.count);
                                } else if (is_double(token) == Y) {
                                    type = Double;
                                    int line1_len = snprintf(NULL, 0, "const double %s[%d] = {%s", vname, tokens.count, token);
                                    char *buffer1 = malloc(line1_len + 1);
                                    sprintf(buffer1, "const double %s[%d] = {%s", vname, tokens.count, token);
                                    global_script = join_str(global_script, buffer1);
                                    free(buffer1);
                                    *params = int_map_set(*params, vname, DoubleList);
                                    *paramSize = int_map_set(*params, vname, tokens.count);
                                } else {
                                    int temp = strlen(token);
                                    if (temp > 1 && token[0] == '"' && token[temp-1] == '"') {
                                        type = String;
                                        int line1_len = snprintf(NULL, 0, "const char *%s[%d] = {%s", vname, tokens.count, token);
                                        char *buffer1 = malloc(line1_len + 1);
                                        sprintf(buffer1, "const char *%s[%d] = {%s", vname, tokens.count, token);
                                        global_script = join_str(global_script, buffer1);
                                        free(buffer1);
                                        *params = int_map_set(*params, vname, StringList);
                                        *paramSize = int_map_set(*params, vname, tokens.count);
                                    } else if (temp > 1 && token[0] == '[' && token[temp-1] == ']') {
                                        is_grid = Y;
                                        char *_token = substr(token, 1, temp-2);
                                        IntList elements = char_index(_token, ',');
                                        if (elements.count == 0) {
                                            free(vname);
                                            clear_int_list(tokens);
                                            clear_int_list(elements);
                                            syslogger(filename, fline_number, UNKNOWN_DATATYPE);
                                            goto cleanup;
                                        }
                                        count = elements.count;
                                        for (int j=0; j<=elements.count; j++) {
                                            if (j == 0) {
                                                char *element = trim(substr(_token, 0, *elements.data[j]));
                                                if (is_integer(element) == Y) {
                                                    int line1_len = snprintf(NULL, 0, "const int %s[%d][%d] = {\n\t{%s", vname, tokens.count, count, element);
                                                    char *buffer1 = malloc(line1_len + 1);
                                                    sprintf(buffer1, "const int %s[%d][%d] = {\n\t{%s", vname, tokens.count, count, element);
                                                    global_script = join_str(global_script, buffer1);
                                                    free(buffer1);
                                                    *params = int_map_set(*params, vname, IntegerGrid);
                                                    *paramSize = int_map_set(*params, vname, tokens.count);
                                                } else if (is_double(element) == Y) {
                                                    type = Double;
                                                    int line1_len = snprintf(NULL, 0, "const double %s[%d][%d] = {\n\t{%s", vname, tokens.count, count, element);
                                                    char *buffer1 = malloc(line1_len + 1);
                                                    sprintf(buffer1, "const double %s[%d][%d] = {\n\t{%s", vname, tokens.count, count, element);
                                                    global_script = join_str(global_script, buffer1);
                                                    free(buffer1);
                                                    *params = int_map_set(*params, vname, DoubleGrid);
                                                    *paramSize = int_map_set(*params, vname, tokens.count);
                                                } else {
                                                    free(vname);
                                                    clear_int_list(tokens);
                                                    clear_int_list(elements);
                                                    syslogger(filename, fline_number, UNKNOWN_DATATYPE);
                                                    goto cleanup;
                                                }
                                            } else if (j < elements.count) {
                                                char *element = trim(substr(_token, *elements.data[j-1] + 1, *elements.data[j] - *elements.data[j-1] - 1));
                                                if (type == Integer && is_integer(element) == N) {
                                                    free(vname);
                                                    clear_int_list(tokens);
                                                    clear_int_list(elements);
                                                    syslogger(filename, fline_number, UNMATCHED_DATATYPE);
                                                    goto cleanup;
                                                } else if (type == Double && is_double(element) == N) {
                                                    free(vname);
                                                    clear_int_list(tokens);
                                                    clear_int_list(elements);
                                                    syslogger(filename, fline_number, UNMATCHED_DATATYPE);
                                                    goto cleanup;
                                                } else {
                                                    int line1_len = snprintf(NULL, 0, ", %s", element);
                                                    char *buffer1 = malloc(line1_len + 1);
                                                    sprintf(buffer1, ", %s", element);
                                                    global_script = join_str(global_script, buffer1);
                                                    free(buffer1);
                                                }
                                            } else {
                                                char *element = trim(substr(_token, *elements.data[j-1] + 1, strlen(_token) - *elements.data[j-1] - 1));
                                                if (type == Integer && is_integer(element) == N) {
                                                    free(vname);
                                                    clear_int_list(tokens);
                                                    clear_int_list(elements);
                                                    syslogger(filename, fline_number, UNMATCHED_DATATYPE);
                                                    goto cleanup;
                                                } else if (type == Double && is_double(element) == N) {
                                                    free(vname);
                                                    clear_int_list(tokens);
                                                    clear_int_list(elements);
                                                    syslogger(filename, fline_number, UNMATCHED_DATATYPE);
                                                    goto cleanup;
                                                } else {
                                                    int line1_len = snprintf(NULL, 0, ", %s", element);
                                                    char *buffer1 = malloc(line1_len + 1);
                                                    sprintf(buffer1, ", %s", element);
                                                    global_script = join_str(global_script, buffer1);
                                                    free(buffer1);
                                                }
                                            }
                                        }
                                    } else {
                                        int *_count = int_map_get(*paramSize, token);
                                        if (_count != NULL) {
                                            count = *_count;
                                            is_grid = Y;
                                        }
                                        int *temp = int_map_get(*params, token);
                                        if (temp == NULL) {
                                            free(vname);
                                            clear_int_list(tokens);
                                            syslogger(filename, fline_number, UNDEFINED_VARIABLE);
                                            goto cleanup;
                                        } else if (*temp == String) {
                                            type = String;
                                            int line1_len = snprintf(NULL, 0, "const char *%s[%d] = {%s};\n", vname, tokens.count, token);
                                            char *buffer1 = malloc(line1_len + 1);
                                            sprintf(buffer1, "const char *%s[%d] = {%s};\n", vname, tokens.count, token);
                                            global_script = join_str(global_script, buffer1);
                                            free(buffer1);
                                        } else if (*temp == Integer) {
                                            int line1_len = snprintf(NULL, 0, "const int %s[%d] = {%s};\n", vname, tokens.count, token);
                                            char *buffer1 = malloc(line1_len + 1);
                                            sprintf(buffer1, "const int %s[%d] = {%s};\n", vname, tokens.count, token);
                                            global_script = join_str(global_script, buffer1);
                                            free(buffer1);
                                        } else if (*temp == Double) {
                                            type = Double;
                                            int line1_len = snprintf(NULL, 0, "const double %s[%d] = {%s};\n", vname, tokens.count, token);
                                            char *buffer1 = malloc(line1_len + 1);
                                            sprintf(buffer1, "const double %s[%d] = {%s};\n", vname, tokens.count, token);
                                            global_script = join_str(global_script, buffer1);
                                            free(buffer1);
                                        } else if (*temp == IntegerList) {
                                            is_grid = Y;
                                            type = IntegerList;
                                            int line1_len = snprintf(NULL, 0, "const int %s[%d][%d] = {\n\t{%s", vname, tokens.count, count, token);
                                            char *buffer1 = malloc(line1_len + 1);
                                            sprintf(buffer1, "const int %s[%d][%d] = {\n\t{%s", vname, tokens.count, count, token);
                                            global_script = join_str(global_script, buffer1);
                                            free(buffer1);
                                        } else if (*temp == DoubleList) {
                                            is_grid = Y;
                                            type = DoubleList;
                                            int line1_len = snprintf(NULL, 0, "const double %s[%d][%d] = {\n\t{%s", vname, tokens.count, count, token);
                                            char *buffer1 = malloc(line1_len + 1);
                                            sprintf(buffer1, "const double %s[%d][%d] = {\n\t{%s", vname, tokens.count, count, token);
                                            global_script = join_str(global_script, buffer1);
                                            free(buffer1);
                                        } else {
                                            free(vname);
                                            clear_int_list(tokens);
                                            syslogger(filename, fline_number, UNKNOWN_DATATYPE);
                                            goto cleanup;
                                        }
                                    }
                                }
                            } else {
                                char *token = (i < tokens.count)
                                    ? trim(substr(data, *tokens.data[i-1] + 1, *tokens.data[i] - *tokens.data[i-1] - 1))
                                    : trim(substr(data, *tokens.data[i-1] + 1, strlen(data) - *tokens.data[i-1] - 1));
                                if (is_grid == N) {
                                    if (type == Integer) {
                                        if (is_integer(token) == N) {
                                            free(vname);
                                            clear_int_list(tokens);
                                            syslogger(filename, fline_number, UNMATCHED_DATATYPE);
                                            goto cleanup;
                                        } else {
                                            int line1_len = snprintf(NULL, 0, ", %s", token);
                                            char *buffer1 = malloc(line1_len + 1);
                                            sprintf(buffer1, ", %s", token);
                                            global_script = join_str(global_script, buffer1);
                                            free(buffer1);
                                        }
                                    } else if (type == Double) {
                                        if (is_double(token) == N) {
                                            free(vname);
                                            clear_int_list(tokens);
                                            syslogger(filename, fline_number, UNMATCHED_DATATYPE);
                                            goto cleanup;
                                        } else {
                                            int line1_len = snprintf(NULL, 0, ", %s", token);
                                            char *buffer1 = malloc(line1_len + 1);
                                            sprintf(buffer1, ", %s", token);
                                            global_script = join_str(global_script, buffer1);
                                            free(buffer1);
                                        }
                                    } else {
                                        int temp = strlen(token);
                                        if (!(temp > 1 && token[0] == '"' && token[temp-1] == '"')) {
                                            free(vname);
                                            clear_int_list(tokens);
                                            syslogger(filename, fline_number, UNMATCHED_DATATYPE);
                                            goto cleanup;
                                        } else {
                                            int line1_len = snprintf(NULL, 0, ", %s", token);
                                            char *buffer1 = malloc(line1_len + 1);
                                            sprintf(buffer1, ", %s", token);
                                            global_script = join_str(global_script, buffer1);
                                            free(buffer1);
                                        }
                                    }
                                } else {
                                    int temp = strlen(token);
                                    if (temp > 1 && token[0] == '[' && token[temp-1] == ']') {
                                        char *_token = substr(token, 1, temp-2);
                                        IntList elements = char_index(_token, ',');
                                        if (elements.count == 0) {
                                            free(vname);
                                            clear_int_list(tokens);
                                            clear_int_list(elements);
                                            syslogger(filename, fline_number, UNKNOWN_DATATYPE);
                                            goto cleanup;
                                        }
                                        if (count != elements.count) {
                                            free(vname);
                                            clear_int_list(tokens);
                                            clear_int_list(elements);
                                            syslogger(filename, fline_number, UNMATCHED_ELEMENTS);
                                            goto cleanup;
                                        }
                                        for (int j=0; j<=elements.count; j++) {
                                            if (j == 0) {
                                                char *element = trim(substr(_token, 0, *elements.data[j]));
                                                if (type == Integer) {
                                                    if (is_integer(element) == N) {
                                                        free(vname);
                                                        clear_int_list(tokens);
                                                        clear_int_list(elements);
                                                        syslogger(filename, fline_number, UNKNOWN_DATATYPE);
                                                        goto cleanup;
                                                    } else {
                                                        int line1_len = snprintf(NULL, 0, "},\n\t{%s", element);
                                                        char *buffer1 = malloc(line1_len + 1);
                                                        sprintf(buffer1, "},\n\t{%s", element);
                                                        global_script = join_str(global_script, buffer1);
                                                        free(buffer1);
                                                    }
                                                } else if (type == Double) {
                                                    if (is_double(element) == N) {
                                                        free(vname);
                                                        clear_int_list(tokens);
                                                        clear_int_list(elements);
                                                        syslogger(filename, fline_number, UNKNOWN_DATATYPE);
                                                        goto cleanup;
                                                    } else {
                                                        int line1_len = snprintf(NULL, 0, "},\n\t{%s", element);
                                                        char *buffer1 = malloc(line1_len + 1);
                                                        sprintf(buffer1, "},\n\t{%s", element);
                                                        global_script = join_str(global_script, buffer1);
                                                        free(buffer1);
                                                    }
                                                }
                                            } else {
                                                char *element = (j < elements.count) ? trim(substr(_token, *elements.data[j-1] + 1, *elements.data[j] - *elements.data[j-1] - 1))
                                                    : trim(substr(_token, *elements.data[j-1] + 1, strlen(_token) - *elements.data[j-1] - 1));
                                                if (type == Integer) {
                                                    if (is_integer(element) == N) {
                                                        free(vname);
                                                        clear_int_list(tokens);
                                                        clear_int_list(elements);
                                                        syslogger(filename, fline_number, UNKNOWN_DATATYPE);
                                                        goto cleanup;
                                                    } else {
                                                        int line1_len = snprintf(NULL, 0, ", %s", element);
                                                        char *buffer1 = malloc(line1_len + 1);
                                                        sprintf(buffer1, ", %s", element);
                                                        global_script = join_str(global_script, buffer1);
                                                        free(buffer1);
                                                    }
                                                } else if (type == Double) {
                                                    if (is_double(element) == N) {
                                                        free(vname);
                                                        clear_int_list(tokens);
                                                        clear_int_list(elements);
                                                        syslogger(filename, fline_number, UNKNOWN_DATATYPE);
                                                        goto cleanup;
                                                    } else {
                                                        int line1_len = snprintf(NULL, 0, ", %s", element);
                                                        char *buffer1 = malloc(line1_len + 1);
                                                        sprintf(buffer1, ", %s", element);
                                                        global_script = join_str(global_script, buffer1);
                                                        free(buffer1);
                                                    }
                                                }
                                            }
                                        }
                                    } else {
                                        if (count != *int_map_get(*paramSize, token)) {
                                            free(vname);
                                            clear_int_list(tokens);
                                            syslogger(filename, fline_number, UNMATCHED_ELEMENTS);
                                            goto cleanup;
                                        }
                                        int *_temp = int_map_get(*params, token);
                                        if (_temp == NULL) {
                                            free(vname);
                                            clear_int_list(tokens);
                                            syslogger(filename, fline_number, UNDEFINED_VARIABLE);
                                            goto cleanup;
                                        } else if (type == Integer && *_temp != IntegerList) {
                                            free(vname);
                                            clear_int_list(tokens);
                                            syslogger(filename, fline_number, UNMATCHED_DATATYPE);
                                            goto cleanup;
                                        } else if (type == Double && *_temp != DoubleList) {
                                            free(vname);
                                            clear_int_list(tokens);
                                            syslogger(filename, fline_number, UNMATCHED_DATATYPE);
                                            goto cleanup;
                                        } else {
                                            int line1_len = snprintf(NULL, 0, "},\n\t{%s", token);
                                            char *buffer1 = malloc(line1_len + 1);
                                            sprintf(buffer1, "},\n\t{%s", token);
                                            global_script = join_str(global_script, buffer1);
                                            free(buffer1);
                                        }
                                    }
                                }
                            }
                        }
                        char *format = (is_grid == Y) ? "}\n};\n" : "};\n";
                        global_script = join_str(global_script, format);
                    }
                    else if (dtype == Map)
                    {
                        syslogger(filename, fline_number, UNKNOWN_DATATYPE);
                        goto cleanup;
                    }
                }
                else if (property_obj.property_type == UnknownProperty) {
                    syslogger(filename, fline_number, UNKNOWN_PROPERTY);
                    goto cleanup;
                }
                break;
            
            case FreeLineText:
                ErrorCode err_cd = analyze_free_line_text(fline, out_file, summary.debug, import_dict, summary.identifier_type, summary.name, *params);
                if (err_cd != NO_ERROR) {
                    syslogger(filename, fline_number, err_cd);
                    goto cleanup;
                }
                break;
        }
    }

    if (text_type != FreeLineText) {
        if (header_script != NULL)
            fputs(header_script, out_file);
        if (summary.identifier_type == Flow)
            fputs(HEADER, out_file);
        if (summary.debug == Y && summary.identifier_type == Flow)
            fputs(HEADER_DEBUG, out_file);
        if (global_script != NULL) {
            fputs(global_script, out_file);
            free(global_script);
            fputs("\n", out_file);
        }
        if (summary.identifier_type != Data) {
            if (import_script != NULL) {
                fwrite(import_script, 1, strlen(import_script), out_file);
                free(import_script);
            }
            fprintf(out_file, "void %s() {\n", summary.name);
            if (summary.debug == Y) {
                if (summary.identifier_type == Flow) {
                    char *flow_s_temp = flow_s();
                    fputs(flow_s_temp, out_file);
                    free(flow_s_temp);
                } else {
                    char *step_s_temp = step_s();
                    fputs(step_s_temp, out_file);
                    free(step_s_temp);
                }
            } else {
                fputs("\n", out_file);
            }
        }
    }

    if (summary.identifier_type != Data) {
        fputs("\t", out_file);
        fputs(print_die(namespace, summary.debug, summary.identifier_type), out_file);
        free(namespace);
        fputs("\n}", out_file);
    }

    cleanup:
        fclose(in_file);
        fclose(out_file);    
        import_dict = clear_str_map(import_dict);

    return summary;
}

// ===== MAIN (END)

// ===== PROVIDER (START)

void transpiler(char *filename) {
    char *cwd = get_cwd(filename);
    delete_old_files(cwd);

    IntMap params = {0, {0, NULL}, {0, NULL}};
    IntMap paramSize = {0, {0, NULL}, {0, NULL}};
    TranspilerSummary summary = transpiler_main(filename, cwd, N, &params, &paramSize);
    free(cwd);
    free(summary.name);
    summary.imports = clear_str_list(summary.imports);
}

// ===== PROVIDER (END)