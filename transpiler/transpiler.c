// ===== EXTERNAL SOURCES (START)

#include "./transpiler.h"

// ===== EXTERNAL SOURCES (END)

// ===== DEFINITIONS (START)

#define HEADER "#include <stdio.h>\n#include <time.h>\n\n"

// ===== DEFINITIONS (END)

// ===== BASIC ENUMS (START)

typedef enum {
    CommentText,
    IdentifierText,
    PropertyText,
    FreeLineText
} TextType;

typedef enum {
    OneLineComment,
    MultiLineComment
} CommentType;

typedef enum {
    Flow,
    Step,
    UnknownIdentifier
} IdentifierType;

typedef enum {
    Param,
    Debug,
    Import,
    UnknownProperty
} PropertyType;

typedef enum {
    Info,
    Error,
    Warning,
    UnknownFunction,
    CustomFunction
} FunctionType;

// ===== BASIC ENUMS (END)

// ===== BASIC STRUCTS (START)

typedef struct {
    PropertyType property_type;
    char *property_value;
} Property;

typedef struct {
    FunctionType function_type;
    char *function_value;
} Function;

typedef struct {
    char *name;
    StrList imports;
    StrList params;
    YesNo debug;
} TranspilerSummary;

// ===== BASIC STRUCTS (END)

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
    }
    return "";
}

void syslogger(char *path, int line, ErrorCode error_code) {
    printf("[Error Path]: %s\n[Error Line]: %i\n[Error Code]: %s\n", path, line, error_code2str(error_code));
    printf("----------------------------------------\n");
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

// ===== UTILS (END)

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

char *get_namespace(char *filename, char *cwd) {
    char *result;
    int pos = strlen(cwd) + 1;
    IntList mlist = char_index(filename, '.');
    int cnt = mlist.data[0][0] - pos;
    result = substr(filename, pos, cnt);
    result = str_replace(result, '\\', '_');
    return result;
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
    if (sValue.count != 1) {
        clear_int_list(sValue);
        return property_obj;
    }

    int offset = strlen(fline) - 1;
    if (fline[offset] != ')') {
        clear_int_list(sValue);
        return property_obj;
    }

    int type_end_pos = *(sValue.data[sValue.count - 1]);
    char *property_type = substr(fline, 1, type_end_pos - 1);
    if (strcmp(property_type, "debug") == 0)
        property_obj.property_type = Debug;
    else if (strcmp(property_type, "import") == 0)
        property_obj.property_type = Import;
    else if (strcmp(property_type, "param") == 0)
        property_obj.property_type = Param;
    else {
        free(property_type);
        clear_int_list(sValue);
        return property_obj;
    }
    free(property_type);

    int pos = type_end_pos + 1;
    int cnt = offset - pos;
    property_obj.property_value = substr(fline, pos, cnt);

    clear_int_list(sValue);

    return property_obj;
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

void read_and_copy_file(const char *full_path_c, char **imported_functions) {
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

    size_t old_len = (*imported_functions) ? strlen(*imported_functions) : 0;
    char *new_str = realloc(*imported_functions, old_len + read_size + 1);
    if (!new_str) {
        free(buffer);
        printf("[Error] Memory allocation failed\n");
        return;
    }

    memcpy(new_str + old_len, buffer, read_size + 1);
    free(buffer);

    *imported_functions = new_str;
}

YesNo is_function(char *fline) {
    IntList sValue = char_index(fline, '(');
    if (sValue.count != 1)
        return N;
    
    int offset = strlen(fline) - 1;
    if (fline[offset] != ')')
        return N;

    return Y;
}

YesNo match_params(char *value, int count) {
    char *temp = str_replace(value, '\'', ' ');
    temp = str_replace(temp, '"', ' ');

    IntList mlist = char_index(temp, ',');
    if (mlist.count+1 != count)
        return N;

    return Y;
}

Function parse_function(char *fline, StrList imports) {
    Function function = {UnknownFunction, NULL};

    int flen = strlen(fline);
    if (fline[flen-1] != ')')
        return function;

    IntList sValue = char_index(fline, '(');
    int pos = sValue.data[0][0]+1;
    int cnt = flen - pos - 1;
    function.function_value = substr(fline, pos, cnt);

    char *temp = substr(fline, 0, sValue.data[0][0]);
    if (strcmp(temp, "Info") == 0)
    {
        if (match_params(function.function_value, 1) == N)
            return function;
        function.function_type = Info;
    }
    else if (strcmp(temp, "Warning") == 0)
    {
        if (match_params(function.function_value, 1) == N)
            return function;
        function.function_type = Warning;
    }
    else if (strcmp(temp, "Error") == 0)
    {
        if (match_params(function.function_value, 1) == N)
            return function;
        function.function_type = Error;
    }
    else {
        for (int i=0; i<imports.count; i++) {
            if (strcmp(temp, imports.data[i]) == 0)
                function.function_type = CustomFunction;
        }
        if (function.function_type == UnknownFunction)
            return function;
    }

    return function;
}

char *form_params(char *params) {
    IntList mlist = char_index(params, ',');
    if (mlist.count == 0) return strdup("");

    char *vname = trim(substr(params, 0, mlist.data[0][0]));
    char *vtype = trim(substr(params, mlist.data[0][0]+1, strlen(params) - mlist.data[0][0]));
    vtype = join_str(vtype, " ");
    vtype = join_str(vtype, vname);

    return vtype;
}

// ===== TOP-DOWN PROCEDURAL GROUPING (END)

// ===== HELPERS (START)

YesNo analyze_free_line_text(char *fline, FILE *out_file, YesNo debug, StrList imports) {
    if (is_function(fline) == N)
        return N;

    Function function = parse_function(fline, imports);
    if (function.function_type == UnknownFunction)
        return N;

    switch (function.function_type) {
        case Info:
            if (debug == N)
                break;
            char *info_temp = info(function.function_value);
            fprintf(out_file, "\t%s", info_temp);
            free(info_temp);
            break;

        case Warning:
            if (debug == N)
                break;
            char *warning_temp = warning(function.function_value);
            fprintf(out_file, "\t%s", warning_temp);
            free(warning_temp);
            break;

        case Error:
            if (debug == N)
                break;
            char *error_temp = error(function.function_value);
            fprintf(out_file, "\t%s", error_temp);
            free(error_temp);
            break;
        
        default:
            fprintf(out_file, "\t%s;\n", fline);
    }

    return Y;
}

// ===== HELPERS (END)

// ===== MAIN (START)

TranspilerSummary transpiler_main(char *filename, char *origin) {
    TranspilerSummary summary = {NULL, {0, NULL}, {0, NULL}, N};
    summary.name = get_namespace(filename, origin);
    
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
    IdentifierType identifier_type = Flow;

    char fline[1024];
    int fline_number = 0;
    char *imported_functions = NULL;
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
                identifier_type = find_identifier(fline);
                if (identifier_type == UnknownIdentifier) {
                    syslogger(filename, fline_number, UNKNOWN_IDENTIFIER);
                    goto cleanup;
                }
                text_type = PropertyText;
                break;
            
            case PropertyText:
                if (is_property(fline) == N) {
                    if (summary.debug == Y && origin != NULL)
                        fputs(HEADER, out_file);
                    if (imported_functions != NULL) {
                        fwrite(imported_functions, 1, strlen(imported_functions), out_file);
                        free(imported_functions);
                    }
                    fprintf(out_file, "void %s(%s) {\n", summary.name, join_str_list(summary.params));
                    if (summary.debug == Y) {
                        if (identifier_type == Flow) {
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
                    if (identifier_type == Flow) {
                        if (analyze_free_line_text(fline, out_file, summary.debug, summary.imports) == N) {
                            syslogger(filename, fline_number, UNKNOWN_FUNCTION);
                            goto cleanup;
                        }
                    } else {
                        fprintf(out_file, "\t%s", out_file);
                    }
                    text_type = FreeLineText;
                    break;
                }
                Property property_obj = parse_property(fline);
                if (property_obj.property_type == Debug) {
                    summary.debug = (strcmp(property_obj.property_value, "true") == 0) ? Y : N;
                }
                else if (property_obj.property_type == Import) {
                    property_obj.property_value = extract_string(property_obj.property_value);
                    char *cwd = get_cwd(filename);
                    char full_path[PATH_MAX];
                    get_full_path(cwd, property_obj.property_value, full_path);
                    if (full_path == NULL) {
                        syslogger(filename, fline_number, FILE_NOT_FOUND);
                        goto cleanup;
                    } else {
                        char *func_cwd = get_cwd(full_path);
                        char *func_namespace = get_namespace(full_path, func_cwd);
                        if (in_str_list(summary.imports, func_namespace) == Y) {
                            free(cwd);
                            free(func_cwd);
                            free(func_namespace);
                            break;
                        }

                        char *full_path_c = get_target_name(full_path);
                        if (access(full_path_c, F_OK) != 0) {
                            TranspilerSummary func_summary = transpiler_main(full_path, origin);
                            if (in_str_list(func_summary.imports, func_namespace) == Y) {
                                free(cwd);
                                free(func_cwd);
                                free(func_namespace);
                                break;
                            }
                        }
                            
                        read_and_copy_file(full_path_c, &imported_functions);
                        summary.imports = append_str_list(summary.imports, func_namespace);
                    }
                    free(cwd);
                }
                else if (property_obj.property_type == Param) {
                    summary.params = append_str_list(summary.params, form_params(property_obj.property_value));
                }
                else if (property_obj.property_type == UnknownProperty) {
                    syslogger(filename, fline_number, UNKNOWN_PROPERTY);
                    goto cleanup;
                }
                break;
            
            case FreeLineText:
                if (identifier_type == Flow) {
                    if (analyze_free_line_text(fline, out_file, summary.debug, summary.imports) == N) {
                        syslogger(filename, fline_number, UNKNOWN_FUNCTION);
                        goto cleanup;
                    }
                } else {
                    fprintf(out_file, "\t%s", out_file);
                }
                break;
        }
    }

    if (text_type != FreeLineText) {
        if (summary.debug == Y && origin != NULL)
            fputs(HEADER, out_file);
        if (imported_functions != NULL) {
            fwrite(imported_functions, 1, strlen(imported_functions), out_file);
            free(imported_functions);
        }
        fprintf(out_file, "void %s(%s) {\n", summary.name, join_str_list(summary.params));
        if (summary.debug == Y) {
            if (identifier_type == Flow) {
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

    if (summary.debug == Y) {
        if (identifier_type == Flow) {
            char *flow_e_temp = flow_e(summary.name);
            fputs(flow_e_temp, out_file);
            free(flow_e_temp);
        } else {
            char *step_e_temp = step_e(summary.name);
            fputs(step_e_temp, out_file);
            free(step_e_temp);
        }
    }
    fputs("}", out_file);

    cleanup:
        fclose(in_file);
        fclose(out_file);

    return summary;
}

// ===== MAIN (END)

// ===== PROVIDER (START)

void transpiler(char *filename) {
    char *cwd = get_cwd(filename);
    delete_old_files(cwd);

    TranspilerSummary summary = transpiler_main(filename, cwd);
    free(summary.name);
    summary.imports = clear_str_list(summary.imports);
    summary.params = clear_str_list(summary.params);
}

// ===== PROVIDER (END)