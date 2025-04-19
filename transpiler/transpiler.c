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
    Data,
    UnknownIdentifier
} IdentifierType;

typedef enum {
    Global,
    Debug,
    Import,
    UnknownProperty
} PropertyType;

typedef enum {
    String,
    Integer,
    Double,
    List,
    StringList,
    IntegerList,
    DoubleList,
    Map,
    StringMap,
    IntegerMap,
    DoubleMap,
    UnknownDataType,
    None
} DataType;

typedef enum {
    EqualTo,
    GreaterThan,
    LesserThan,
    GreaterOrEqual,
    LesserOrEqual,
    UnknownComparator
} ComparatorType;

typedef enum {
    StringLiteral,
    IntegerStatic,
    DoubleStatic,
    StringVar,
    IntegerVar,
    DoubleVar,
    UnknownComparatorDataType
} ComparatorDataType;

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

typedef struct {
    ComparatorType type;
    char *value;
} Comparator;

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
        case INVALID_IMPORT:
            return "INVALID_IMPORT";
        case UNDEFINED_VARIABLE:
            return "UNDEFINED_VARIABLE";
        case INVALID_VARIABLE:
            return "INVALID_VARIABLE";
        case ILLEGAL_DATATYPE:
            return "ILLEGAL_DATATYPE";
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
                    if (summary.identifier_type == Data) {
                        syslogger(filename, fline_number, UNKNOWN_PROPERTY);
                        goto cleanup;
                    }
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
                                TranspilerSummary func_summary = transpiler_main(full_path, origin, summary.debug, params);
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
                        property_obj.property_value = join_str(property_obj.property_value, "\\main.link");
                        get_full_path(cwd, property_obj.property_value, full_path);
                        if (full_path == NULL) {
                            syslogger(filename, fline_number, FILE_NOT_FOUND);
                            goto cleanup;
                        }

                        if (access(full_path, F_OK) != 0) {
                            syslogger(filename, fline_number, FILE_NOT_FOUND);
                            goto cleanup;
                        }

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
                        char *vname = trim(mlist.data[0]);
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
                    else if (dtype == Double)
                    {
                        char *vname = trim(mlist.data[0]);
                        if (int_map_get(*params, vname) != NULL) {
                            syslogger(filename, fline_number, PREDEFINED_VARIABLE);
                            goto cleanup;
                        }

                        int line1_len = snprintf(NULL, 0, "double %s = %s;\n", vname, mlist.data[1]);
                        char *buffer1 = malloc(line1_len + 1);
                        sprintf(buffer1, "double %s = %s;\n", vname, mlist.data[1]);
                        global_script = join_str(global_script, buffer1);
                        free(buffer1);

                        *params = int_map_set(*params, vname, Integer);
                        free(vname);
                    }
                    else if (dtype == List)
                    {
                        char *vname = trim(mlist.data[0]);
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
                        else if (ltype == Double) {
                            StrList elements = str2list(substr(mlist.data[1], 1, strlen(mlist.data[1])-2));

                            int line1_len = snprintf(NULL, 0, "double *%s = malloc(%i * sizeof(double));\n", vname, elements.count);
                            char *buffer1 = malloc(line1_len + 1);
                            sprintf(buffer1, "double *%s = malloc(%i * sizeof(double));\n", vname, elements.count);
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
                    else if (dtype == Map)
                    {
                        char *vname = trim(mlist.data[0]);
                        if (int_map_get(*params, vname) != NULL) {
                            syslogger(filename, fline_number, PREDEFINED_VARIABLE);
                            goto cleanup;
                        }

                        StrList map_args = parse_map(mlist.data[1]);
                        if (map_args.count == 0) {
                            syslogger(filename, fline_number, UNKNOWN_DATATYPE);
                            goto cleanup;
                        }

                        int line_len = snprintf(NULL, 0, "void **%s = malloc(2 * sizeof(void*));\n", vname);
                        char *buffer = malloc(line_len + 1);
                        sprintf(buffer, "void **%s = malloc(2 * sizeof(void*));\n", vname);
                        global_script = join_str(global_script, buffer);

                        DataType dtype = get_dtype(map_args.data[0]);
                        if (dtype == UnknownDataType) {
                            int *_dtype = int_map_get(*params, map_args.data[0]);
                            if (_dtype == NULL || *_dtype != StringList) {
                                syslogger(filename, fline_number, ILLEGAL_DATATYPE);
                                goto cleanup;
                            }
                            line_len = snprintf(NULL, 0, "%s[0] = %s;\n", vname, map_args.data[0]);
                            buffer = realloc(buffer, line_len + 1);
                            sprintf(buffer, "%s[0] = %s;\n", vname, map_args.data[0]);
                            global_script = join_str(global_script, buffer);
                        } else if (dtype == List) {
                            DataType ltype = get_ltype(map_args.data[0]);
                            if (ltype != String) {
                                syslogger(filename, fline_number, ILLEGAL_DATATYPE);
                                goto cleanup;
                            }
                            StrList elements1 = str2list(substr(map_args.data[1], 1, strlen(map_args.data[1]) - 2));
                            if (dtype == String) {
                                line_len = snprintf(NULL, 0, "%s[1] = malloc(%i * sizeof(char*));\n", vname, elements1.count);
                                buffer = realloc(buffer, line_len + 1);
                                sprintf(buffer, "%s[1] = malloc(%i * sizeof(char*));\n", vname, elements1.count);
                                global_script = join_str(global_script, buffer);
                                for (int i=0; i<elements1.count; i++) {
                                    line_len = snprintf(NULL, 0, "%s[1][%i] = strdup(\"%s\");\n", vname, i, elements1.data[i]);
                                    buffer = realloc(buffer, line_len + 1);
                                    sprintf(buffer, "%s[1][%i] = strdup(\"%s\");\n", vname, i, elements1.data[i]);
                                    global_script = join_str(global_script, buffer);
                                }
                            } else if (dtype == Integer) {
                                line_len = snprintf(NULL, 0, "%s[1] = malloc(%i * sizeof(int));\n", vname, elements1.count);
                                buffer = realloc(buffer, line_len + 1);
                                sprintf(buffer, "%s[1] = malloc(%i * sizeof(int));\n", vname, elements1.count);
                                global_script = join_str(global_script, buffer);
                                for (int i=0; i<elements1.count; i++) {
                                    line_len = snprintf(NULL, 0, "%s[1][%i] = %s;\n", vname, i, elements1.data[i]);
                                    buffer = realloc(buffer, line_len + 1);
                                    sprintf(buffer, "%s[1][%i] = %s;\n", vname, i, elements1.data[i]);
                                    global_script = join_str(global_script, buffer);
                                }
                            } else if (dtype == Double) {
                                line_len = snprintf(NULL, 0, "%s[1] = malloc(%i * sizeof(double));\n", vname, elements1.count);
                                buffer = realloc(buffer, line_len + 1);
                                sprintf(buffer, "%s[1] = malloc(%i * sizeof(double));\n", vname, elements1.count);
                                global_script = join_str(global_script, buffer);
                                for (int i=0; i<elements1.count; i++) {
                                    line_len = snprintf(NULL, 0, "%s[1][%i] = %s;\n", vname, i, elements1.data[i]);
                                    buffer = realloc(buffer, line_len + 1);
                                    sprintf(buffer, "%s[1][%i] = %s;\n", vname, i, elements1.data[i]);
                                    global_script = join_str(global_script, buffer);
                                }
                            }
                        } else {
                            syslogger(filename, fline_number, ILLEGAL_DATATYPE);
                            goto cleanup;
                        }

                        dtype = get_dtype(map_args.data[1]);
                        if (dtype == UnknownDataType) {
                            int *_dtype = int_map_get(*params, map_args.data[1]);
                            if (_dtype == NULL || (*_dtype != StringList && *_dtype != IntegerList && *_dtype != DoubleList)) {
                                syslogger(filename, fline_number, ILLEGAL_DATATYPE);
                                goto cleanup;
                            }
                            line_len = snprintf(NULL, 0, "%s[1] = %s;\n", vname, map_args.data[1]);
                            buffer = realloc(buffer, line_len + 1);
                            sprintf(buffer, "%s[1] = %s;\n", vname, map_args.data[1]);
                            global_script = join_str(global_script, buffer);
                        } else if (dtype == List) {
                            dtype = get_ltype(map_args.data[1]);
                            if (dtype == UnknownDataType) {
                                syslogger(filename, fline_number, ILLEGAL_DATATYPE);
                                goto cleanup;
                            }
                            StrList elements2 = str2list(substr(map_args.data[1], 1, strlen(map_args.data[1]) - 2));
                            if (dtype == String) {
                                line_len = snprintf(NULL, 0, "%s[1] = malloc(%i * sizeof(char*));\n", vname, elements2.count);
                                buffer = realloc(buffer, line_len + 1);
                                sprintf(buffer, "%s[1] = malloc(%i * sizeof(char*));\n", vname, elements2.count);
                                global_script = join_str(global_script, buffer);
                                for (int i=0; i<elements2.count; i++) {
                                    line_len = snprintf(NULL, 0, "%s[1][%i] = strdup(\"%s\");\n", vname, i, elements2.data[i]);
                                    buffer = realloc(buffer, line_len + 1);
                                    sprintf(buffer, "%s[1][%i] = strdup(\"%s\");\n", vname, i, elements2.data[i]);
                                    global_script = join_str(global_script, buffer);
                                }
                            } else if (dtype == Integer) {
                                line_len = snprintf(NULL, 0, "%s[1] = malloc(%i * sizeof(int));\n", vname, elements2.count);
                                buffer = realloc(buffer, line_len + 1);
                                sprintf(buffer, "%s[1] = malloc(%i * sizeof(int));\n", vname, elements2.count);
                                global_script = join_str(global_script, buffer);
                                for (int i=0; i<elements2.count; i++) {
                                    line_len = snprintf(NULL, 0, "%s[1][%i] = %s;\n", vname, i, elements2.data[i]);
                                    buffer = realloc(buffer, line_len + 1);
                                    sprintf(buffer, "%s[1][%i] = %s;\n", vname, i, elements2.data[i]);
                                    global_script = join_str(global_script, buffer);
                                }
                            } else if (dtype == Double) {
                                line_len = snprintf(NULL, 0, "%s[1] = malloc(%i * sizeof(double));\n", vname, elements2.count);
                                buffer = realloc(buffer, line_len + 1);
                                sprintf(buffer, "%s[1] = malloc(%i * sizeof(double));\n", vname, elements2.count);
                                global_script = join_str(global_script, buffer);
                                for (int i=0; i<elements2.count; i++) {
                                    line_len = snprintf(NULL, 0, "%s[1][%i] = %s;\n", vname, i, elements2.data[i]);
                                    buffer = realloc(buffer, line_len + 1);
                                    sprintf(buffer, "%s[1][%i] = %s;\n", vname, i, elements2.data[i]);
                                    global_script = join_str(global_script, buffer);
                                }
                            }
                        } else {
                            syslogger(filename, fline_number, ILLEGAL_DATATYPE);
                            goto cleanup;
                        }

                        *params = int_map_set(*params, vname, dtype);
                        free(buffer);
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
    TranspilerSummary summary = transpiler_main(filename, cwd, N, &params);
    free(cwd);
    free(summary.name);
    summary.imports = clear_str_list(summary.imports);
}

// ===== PROVIDER (END)