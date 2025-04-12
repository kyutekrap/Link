// ===== EXTERNAL SOURCES (START)

#include "./transpiler.h"

// ===== EXTERNAL SOURCES (END)

// ===== DEFINITIONS (START)

#define HEADER "#include <stdlib.h>\n#include <string.h>\n"
#define HEADER_DEBUG "#include <stdio.h>\n#include <time.h>\n\n"

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
    Set,
    Get,
    Debug,
    Import,
    UnknownProperty
} PropertyType;

typedef enum {
    String,
    Integer,
    List,
    StringList,
    IntegerList,
    UnknownDataType,
    None
} DataType;

// ===== BASIC ENUMS (END)

// ===== BASIC STRUCTS (START)

typedef struct {
    PropertyType property_type;
    char *property_value;
} Property;

typedef struct {
    char *function_type;
    char *function_value;
} Function;

typedef struct {
    char *name;
    StrList imports;
    YesNo debug;
    IdentifierType identifier_type;
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
        case UNKNOWN_DATATYPE:
            return "UNKNOWN_DATATYPE";
        case INVALID_FILENAME:
            return "INVALID_FILENAME";
        case PREDEFINED_VARIABLE:
            return "PREDEFINED_VARIABLE";
        case UNDEFINED_STEP:
            return "UNDEFINED_STEP";
        case UNDEFINED_VARIABLE:
            return "UNDEFINED_VARIABLE";
    }
    return strdup("");
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

YesNo is_number(char *var) {
    if (var == NULL || *var == '\0')
        return N;

    for (int i = 0; var[i] != '\0'; i++) {
        if (!isdigit((unsigned char)var[i]))
            return N;
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
            } else if (ltype = Integer) {
                ltype = UnknownDataType;
                break;
            }
        } else if (is_number(current) == Y) {
            if (ltype = UnknownDataType) {
                ltype = Integer;
            } else if (ltype = String) {
                ltype = UnknownDataType;
                break;
            }
        } else {
            break;
        }
    }

    return ltype;
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
    else if (strcmp(property_type, "set") == 0)
        property_obj.property_type = Set;
    else if (strcmp(property_type, "get") == 0)
        property_obj.property_type = Get;
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

DataType get_dtype(char *str) {
    int str_len = strlen(str);
    if (str_len == 0)
    {
        return None;
    }
    else if (is_number(str) == Y)
    {
        return Integer;
    }
    else if (str_len >= 2 && str[0] == '"' && str[str_len-1] == '"')
    {
        return String;
    }
    else if (str_len >= 2 && str[0] == '[' && str[str_len-1] == ']')
    {
        return List;
    }

    return UnknownDataType;
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
        if (debug == Y) {
            if (identifier_type == Flow) {
                char *flow_e_temp = flow_e(namespace);
                fputs(flow_e_temp, out_file);
                free(flow_e_temp);
            } else {
                char *step_e_temp = step_e(namespace);
                fputs(step_e_temp, out_file);
                free(step_e_temp);
            }
        }
        fputs("\treturn;\n", out_file);
    }
    else if (strcmp(function.function_type, "set") == 0)
    {
        StrList mlist = str2list(function.function_value);
        if (get_dtype(mlist.data[0]) != String)
            return UNKNOWN_FUNCTION;

        DataType dtype = get_dtype(mlist.data[1]);
        if (dtype == UnknownDataType)
            return UNKNOWN_FUNCTION;
    }
    else if (strcmp(function.function_type, "get") == 0)
    {
        StrList mlist = str2list(function.function_value);
        if (get_dtype(mlist.data[0]) != String)
            return UNKNOWN_FUNCTION;
    }
    else
    {
        fprintf(out_file, "\t%s(%s);\n", function.function_type, function.function_value);
    }

    return NO_ERROR;
}

// ===== HELPERS (END)

// ===== MAIN (START)

TranspilerSummary transpiler_main(char *filename, char *origin, YesNo debug, IntMap *params) {
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
                    if (err_cd != NO_ERROR) {
                        syslogger(filename, fline_number, err_cd);
                        goto cleanup;
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
                            TranspilerSummary func_summary = transpiler_main(full_path, origin, summary.debug, params);
                            if (func_summary.identifier_type != Step) {
                                free(cwd);
                                free(func_namespace);
                                syslogger(filename, fline_number, UNDEFINED_STEP);
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
                        import_dict = str_map_set(import_dict, get_namespace(full_path, func_cwd), func_namespace);
                    }
                    free(cwd);
                }
                else if (property_obj.property_type == Set && summary.identifier_type == Flow) {
                    StrList mlist = str2list(property_obj.property_value);
                    if (mlist.count != 2) {
                        syslogger(filename, fline_number, UNKNOWN_PROPERTY);
                        goto cleanup;
                    }

                    if (get_dtype(mlist.data[0]) != String) {
                        syslogger(filename, fline_number, UNKNOWN_PROPERTY);
                        goto cleanup;
                    }

                    DataType dtype = get_dtype(mlist.data[1]);
                    if (dtype == String)
                    {
                        char *vname = extract_string(mlist.data[0]);
                        if (int_map_get(*params, vname) != NULL) {
                            syslogger(filename, fline_number, PREDEFINED_VARIABLE);
                            goto cleanup;
                        }

                        size_t str_len = strlen(extract_string(mlist.data[1])) + 1;
                        int line1_len = snprintf(NULL, 0, "char *%s = malloc(%zu);\n", vname, str_len);
                        char *buffer1 = malloc(line1_len + 1);
                        sprintf(buffer1, "char *%s = malloc(%zu);\n", vname, str_len);
                        global_script = join_str(global_script, buffer1);
                        free(buffer1);

                        int line2_len = snprintf(NULL, 0, "strcpy(%s, %s);\n", vname, mlist.data[1]);
                        char *buffer2 = malloc(line2_len + 1);
                        sprintf(buffer2, "strcpy(%s, %s);\n", vname, mlist.data[1]);
                        global_script = join_str(global_script, buffer2);
                        free(buffer2);

                        *params = int_map_set(*params, vname, String);
                        free(vname);
                    }
                    else if (dtype == Integer)
                    {
                        char *vname = extract_string(mlist.data[0]);
                        if (int_map_get(*params, vname) != NULL) {
                            syslogger(filename, fline_number, PREDEFINED_VARIABLE);
                            goto cleanup;
                        }

                        int line1_len = snprintf(NULL, 0, "int %s = %s;\n", vname, mlist.data[1]);
                        char *buffer1 = malloc(line1_len + 1);
                        sprintf(buffer1, "int %s = %s;\n", vname, mlist.data[1]);
                        global_script = join_str(global_script, buffer1);
                        free(buffer1);

                        *params = int_map_set(*params, vname, Integer);
                        free(vname);
                    }
                    else if (dtype == List)
                    {
                        char *vname = extract_string(mlist.data[0]);
                        if (int_map_get(*params, vname) != NULL) {
                            syslogger(filename, fline_number, PREDEFINED_VARIABLE);
                            goto cleanup;
                        }

                        DataType ltype = get_ltype(mlist.data[1]);
                        if (ltype == String) {
                            StrList elements = str2list(substr(mlist.data[1], 1, strlen(mlist.data[1])-2));
                            
                            int line1_len = snprintf(NULL, 0, "char **%s = malloc(%i * sizeof(char*));\n", vname, elements.count);
                            char *buffer1 = malloc(line1_len + 1);
                            sprintf(buffer1, "char **%s = malloc(%i * sizeof(char*));\n", vname, elements.count);
                            global_script = join_str(global_script, buffer1);
                            free(buffer1);

                            for (int i=0; i<elements.count; i++) {
                                int line2_len = snprintf(NULL, 0, "%s[%i] = strdup(%s);\n", vname, i, elements.data[i]);
                                char *buffer2 = malloc(line1_len + 1);
                                sprintf(buffer2, "%s[%i] = strdup(%s);\n", vname, i, elements.data[i]);
                                global_script = join_str(global_script, buffer2);
                                free(buffer2);
                            }

                            *params = int_map_set(*params, vname, StringList);
                            free(vname);
                        }
                        else if (ltype == Integer) {
                            StrList elements = str2list(substr(mlist.data[1], 1, strlen(mlist.data[1])-2));

                            int line1_len = snprintf(NULL, 0, "int *%s = malloc(%i * sizeof(int));\n", vname, elements.count);
                            char *buffer1 = malloc(line1_len + 1);
                            sprintf(buffer1, "int *%s = malloc(%i * sizeof(int));\n", vname, elements.count);
                            global_script = join_str(global_script, buffer1);
                            free(buffer1);

                            for (int i=0; i<elements.count; i++) {
                                int line2_len = snprintf(NULL, 0, "%s[%i] = %s;\n", vname, i, elements.data[i]);
                                char *buffer2 = malloc(line1_len + 1);
                                sprintf(buffer2, "%s[%i] = %s;\n", vname, i, elements.data[i]);
                                global_script = join_str(global_script, buffer2);
                                free(buffer2);
                            }

                            *params = int_map_set(*params, vname, IntegerList);
                            free(vname);
                        }
                        else {
                            syslogger(filename, fline_number, UNKNOWN_DATATYPE);
                            free(vname);
                            goto cleanup;
                        }
                    }
                    else {
                        syslogger(filename, fline_number, UNKNOWN_DATATYPE);
                        goto cleanup;
                    }
                }
                else if (property_obj.property_type == Get && summary.identifier_type == Step) {
                    StrList mlist = str2list(property_obj.property_value);
                    if (mlist.count != 1) {
                        syslogger(filename, fline_number, UNKNOWN_PROPERTY);
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
    }

    if (summary.debug == Y) {
        if (summary.identifier_type == Flow) {
            char *flow_e_temp = flow_e(namespace);
            fputs(flow_e_temp, out_file);
            free(flow_e_temp);
        } else {
            char *step_e_temp = step_e(namespace);
            fputs(step_e_temp, out_file);
            free(step_e_temp);
        }
    }
    free(namespace);
    fputs("}", out_file);

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
    TranspilerSummary summary = transpiler_main(filename, cwd, N, &params);
    free(cwd);
    free(summary.name);
    summary.imports = clear_str_list(summary.imports);
}

// ===== PROVIDER (END)