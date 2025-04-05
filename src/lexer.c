// ===== EXTERNAL SOURCES (START)

#include "./lexer.h"

// ===== EXTERNAL SOURCES (END)

// ===== BASIC ENUMS (START)

typedef enum {
    Y,
    N
} YesNo;

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
    DebugEnv,
    LiveEnv
} EnvironType;

// ===== BASIC ENUMS (END)

// ===== BASIC STRUCTS (START)

typedef struct {
    PropertyType property_type;
    char *property_value;
} Property;

typedef struct {
    int count;
    int **data;
} IntList;

typedef struct {
    int count;
    char **data;
} StrList;

// ===== BASIC STRUCTS (END)

// ===== UTILS (START)

void *trim(char *str) {
    char *start = str;
    char *end;
    while (isspace((unsigned char)*start)) {
        start++;
    }
    if (*start == '\0') {
        str[0] = '\0';
        return str;
    }
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';
    memmove(str, start, end - start + 2);
}

char *substr(const char *str, int pos, int cnt) {
    int len = strlen(str);
    if (pos < 0 || pos >= len || cnt < 0) return NULL;
    if (pos + cnt > len) {
        cnt = len - pos;
    }
    char *temp = malloc(cnt + 1);
    if (!temp) return NULL;
    strncpy(temp, str + pos, cnt);
    temp[cnt] = '\0';
    return temp;
}

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
        case UNSUPPORTED_DTYPE:
            return "UNSUPPORTED_DTYPE";
        case FILENAME_TOO_LONG:
            return "FILENAME_TOO_LONG";
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

IntList char_index(const char *str, char target) {
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

YesNo is_main_file(char *filename) {
    const int cnt = 4;
    const int offset = 5;
    const int pos = strlen(filename) - cnt - offset;
    if (pos < 1)
        return N;
    if (strcmp(substr(filename, pos, cnt), "main") == 0)
        return Y;
    else
        return N;
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

char *get_namespace(char *filename) {
    IntList mlist = char_index(filename, '\\');
    if (mlist.count == 0) {
        return NULL;
    }

    int pos = *(mlist.data[mlist.count - 1]) + 1;
    int offset = 5;
    int cnt = strlen(filename) - pos - offset;

    char *result = substr(filename, pos, cnt);

    for (int i = 0; i < mlist.count; i++) {
        free(mlist.data[i]);
    }
    free(mlist.data);

    return result;
}

char *get_target_name(const char *filename) {
    int len = strlen(filename);
    char *new_str = malloc(len - 3);
    if (!new_str) return NULL;
    strncpy(new_str, filename, len - 4);
    new_str[len - 4] = 'c';
    new_str[len - 3] = '\0';
    return new_str;
}

void delete_old_file(char *filename) {
    remove(filename);
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
        for (int i = 0; i < sValue.count; i++) {
            free(sValue.data[i]);
        }
        free(sValue.data);
        return property_obj;
    }

    int offset = strlen(fline) - 1;
    if (fline[offset] != ')') {
        for (int i = 0; i < sValue.count; i++) {
            free(sValue.data[i]);
        }
        free(sValue.data);
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
        for (int i = 0; i < sValue.count; i++) {
            free(sValue.data[i]);
        }
        free(sValue.data);
        return property_obj;
    }
    free(property_type);

    int pos = type_end_pos + 1;
    int cnt = offset - pos;
    property_obj.property_value = substr(fline, pos, cnt);

    for (int i = 0; i < sValue.count; i++) {
        free(sValue.data[i]);
    }
    free(sValue.data);

    return property_obj;
}

char *join_str(char *original_str, const char *new_str) {
    if (original_str == NULL) {
        original_str = malloc(strlen(new_str) + 1);
        if (!original_str) {
            printf("[Error] Memory allocation failed\n");
            return NULL;
        }
        strcpy(original_str, new_str);
        return original_str;
    }

    size_t new_len = strlen(original_str) + strlen(new_str) + 1;
    char *temp = realloc(original_str, new_len);
    if (!temp) {
        printf("[Error] Memory allocation failed\n");
        return original_str;
    }

    original_str = temp;
    strcat(original_str, new_str);
    return original_str;
}

StrList search_files(char *cwd) {
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
            if (ext && strcmp(ext, ".link") == 0) {
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

// ===== TOP-DOWN PROCEDURAL GROUPING (END)

// ===== MAIN (START)

void lexer(char *filename) {
    if (is_link_file(filename) == N) {
        syslogger(filename, 0, INVALID_FILE);
        return;
    }

    YesNo is_main = is_main_file(filename);
    char *cwd = get_cwd(filename);
    char *namespace = get_namespace(filename);

    char *target_name = get_target_name(filename);
    delete_old_file(target_name);

    FILE *in_file = fopen(filename, "r");
    if (in_file == NULL) {
        syslogger(filename, 0, FILE_NOT_FOUND);
        return;
    }

    FILE *out_file = fopen(target_name, "w");
    if (out_file == NULL) {
        fclose(in_file);
        syslogger(filename, 0, C_COMPILE_ERROR);
        return;
    }
    
    TextType text_type = IdentifierText;
    CommentType comment_type = OneLineComment;
    IdentifierType identifier_type = Flow;
    EnvironType environ_type = DebugEnv;

    char fline[1024];
    int fline_number = 0;
    char *collected_params = NULL;
    while (fgets(fline, sizeof(fline), in_file)) {
        fline_number ++;

        if (is_empty(fline) == Y)
            continue;
    
        if (text_type != FreeLineText) {
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
                    if (environ_type == DebugEnv)
                        fputs(HEADER, out_file);
                    if (collected_params == NULL)
                        fprintf(out_file, "void %s() {\n", namespace);
                    else
                        fprintf(out_file, "void %s(%s) {\n", namespace, collected_params);
                    if (environ_type == DebugEnv)
                        fputs(identifier_type == Flow ? FLOW_S : STEP_S, out_file);
                    else
                        fputs("\n", out_file);
                    fprintf(out_file, "\t%s", out_file);
                    text_type = FreeLineText;
                    break;
                }
                Property property_obj = parse_property(fline);
                if (property_obj.property_type == Debug) {
                    environ_type = (strcmp(property_obj.property_value, "true") == 0) ? DebugEnv : LiveEnv;
                }
                else if (property_obj.property_type == Import) {
                    if (is_main == Y)
                        break;

                    property_obj.property_value = extract_string(property_obj.property_value);
                    fprintf(out_file, "#include \"%s\"\n", get_target_name(property_obj.property_value));

                    char full_path[PATH_MAX];
                    get_full_path(cwd, property_obj.property_value, full_path);
                    if (full_path == NULL)
                        syslogger(filename, fline_number, FILE_NOT_FOUND);
                    else
                        lexer(full_path);
                }
                else if (property_obj.property_type == Param) {
                    collected_params = join_str(collected_params, property_obj.property_value);
                }
                else if (property_obj.property_type == UnknownProperty) {
                    syslogger(filename, fline_number, UNKNOWN_PROPERTY);
                    goto cleanup;
                }
                break;
            
            case FreeLineText:
                fprintf(out_file, "\t%s", out_file);
                break;
        }
    }

    if (text_type != FreeLineText) {
        if (environ_type == DebugEnv)
            fputs(HEADER, out_file);
        if (collected_params == NULL)
            fprintf(out_file, "void %s() {\n", namespace);
        else
            fprintf(out_file, "void %s(%s) {\n", namespace, collected_params);
        if (environ_type == DebugEnv)
            fputs(identifier_type == Flow ? FLOW_S : STEP_S, out_file);
        else
            fputs("\n", out_file);
    }

    if (environ_type == DebugEnv)
        fputs(identifier_type == Flow ? FLOW_E : STEP_E, out_file);
    fputs("}", out_file);

    if (is_main == Y) {
        StrList files = search_files(cwd);
        for (int i = 0; i < files.count; i ++) {
            if (strcmp(files.data[i], filename) == 0)
                continue;
            if (is_link_file(files.data[i]) == Y);
                lexer(files.data[i]);
        }
        StrList dirs = search_dirs(cwd);
        for (int i = 0; i < dirs.count; i ++) {
            StrList files = search_files(dirs.data[i]);
            for (int i = 0; i < files.count; i ++) {
                if (is_link_file(files.data[i]) == Y);
                    lexer(files.data[i]);
            }
        }
    }

    cleanup:
        fclose(in_file);
        fclose(out_file);
}

// ===== MAIN (END)