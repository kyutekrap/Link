#include "./utils.h"

// ===== STRING UTILS (START)

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

char *substr(char *str, int pos, int cnt) {
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

char *join_str(char *original_str, char *new_str) {
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

char *str_replace(char *str, char from_char, char to_char) {
    if (!str) return NULL;

    size_t len = strlen(str);
    char *new_str = malloc(len + 1);
    if (!new_str) return NULL;

    for (size_t i = 0; i < len; i++) {
        new_str[i] = (str[i] == from_char) ? to_char : str[i];
    }

    new_str[len] = '\0';
    return new_str;
}

// ===== STRING UTILS (END)

// ===== LIST UTILS (START)

IntList append_int_list(IntList int_list, int new_value) {
    int **new_data = realloc(int_list.data, (int_list.count + 1) * sizeof(int *));
    if (!new_data) {
        printf("[Error] Memory allocation failed\n");
        return int_list;
    }

    int *value_ptr = malloc(sizeof(int));
    if (!value_ptr) {
        printf("[Error] Memory allocation failed\n");
        return int_list;
    }

    *value_ptr = new_value;
    new_data[int_list.count] = value_ptr;

    int_list.data = new_data;
    int_list.count++;

    return int_list;
}

StrList append_str_list(StrList str_list, char *new_value) {
    str_list.count++;
    str_list.data = realloc(str_list.data, str_list.count * sizeof(char *));
    if (str_list.data)
        str_list.data[str_list.count-1] = new_value;
    else
        printf("[Error] Memory allocation failed\n");
    return str_list;
}

YesNo in_str_list(StrList str_list, char *checking_value) {
    for (int i=0; i<str_list.count; i++) {
        if (strcmp(str_list.data[i], checking_value) == 0) {
            return Y;
        }
    }
    return N;
}

YesNo in_int_list(IntList int_list, int checking_value) {
    for (int i = 0; i < int_list.count; i++) {
        if (*(int_list.data[i]) == checking_value) {
            return Y;
        }
    }
    return N;
}

StrList clear_str_list(StrList mlist) {
    for (int j = 0; j < mlist.count; j++) {
        free(mlist.data[j]);
    }
    free(mlist.data);
    mlist.data = NULL;
    mlist.count = 0;
    return mlist;
}

IntList clear_int_list(IntList mlist) {
    for (int j = 0; j < mlist.count; j++) {
        free(mlist.data[j]);
    }
    free(mlist.data);
    mlist.data = NULL;
    mlist.count = 0;
    return mlist;
}

char *join_str_list(StrList mlist) {
    if (mlist.count == 0) return strdup("");

    size_t total_len = 0;
    int sep_len = 2;

    for (int i = 0; i < mlist.count; i++) {
        if (mlist.data[i]) {
            total_len += strlen(mlist.data[i]);
        }
    }

    if (mlist.count > 1) {
        total_len += sep_len * (mlist.count - 1);
    }

    char *str = malloc(total_len + 1);
    if (!str) return NULL;

    char *ptr = str;
    for (int i = 0; i < mlist.count; i++) {
        if (mlist.data[i]) {
            size_t len = strlen(mlist.data[i]);
            memcpy(ptr, mlist.data[i], len);
            ptr += len;

            if (i < mlist.count - 1) {
                memcpy(ptr, ", ", sep_len);
                ptr += sep_len;
            }
        }
    }

    *ptr = '\0';
    return str;
}

StrList str2list(char *str) {
    StrList mlist = {0, NULL};
    const int value_len = strlen(str);
    if (value_len == 0)
        return mlist;

    int start = 0;
    int in_quotes = 0;
    int bracket_depth = 0;
    int brace_depth = 0;

    for (int i = 0; i <= value_len; ++i) {
        char c = str[i];

        if (c == '"') {
            in_quotes = !in_quotes;
        } else if (!in_quotes) {
            if (c == '[') {
                bracket_depth++;
            } else if (c == ']') {
                if (bracket_depth > 0)
                    bracket_depth--;
            } else if (c == '{') {
                brace_depth++;
            } else if (c == '}') {
                if (brace_depth > 0)
                    brace_depth--;
            } else if ((c == ',' || c == '\0') && bracket_depth == 0 && brace_depth == 0) {
                int token_len = i - start;

                while (token_len > 0 && isspace(str[start])) {
                    start++;
                    token_len--;
                }

                while (token_len > 0 && isspace(str[start + token_len - 1])) {
                    token_len--;
                }

                if (token_len > 0) {
                    char *token = (char *)malloc(token_len + 1);
                    strncpy(token, str + start, token_len);
                    token[token_len] = '\0';

                    mlist.data = (char **)realloc(mlist.data, sizeof(char *) * (mlist.count + 1));
                    mlist.data[mlist.count++] = token;
                }

                start = i + 1;
            }
        }
    }

    return mlist;
}

// ===== LIST UTILS (END)

// ===== MAP UTILS (START)

StrMap str_map_set(StrMap str_map, char *key, char *value) {
    for (int i = 0; i < str_map.count; ++i) {
        if (strcmp(str_map.keys.data[i], key) == 0) {
            free(str_map.values.data[i]);
            str_map.values.data[i] = strdup(value);
            return str_map;
        }
    }

    str_map.count++;

    str_map.keys.data = realloc(str_map.keys.data, str_map.count * sizeof(char *));
    str_map.values.data = realloc(str_map.values.data, str_map.count * sizeof(char *));

    str_map.keys.data[str_map.count - 1] = strdup(key);
    str_map.values.data[str_map.count - 1] = strdup(value);

    str_map.keys.count = str_map.count;
    str_map.values.count = str_map.count;

    return str_map;
}

char *str_map_get(StrMap str_map, char *key) {
    for (int i = 0; i < str_map.count; ++i) {
        if (strcmp(str_map.keys.data[i], key) == 0) {
            return str_map.values.data[i];
        }
    }
    return NULL;
}

StrMap clear_str_map(StrMap str_map) {
    clear_str_list(str_map.keys);
    clear_str_list(str_map.values);
    str_map.count = 0;
    return str_map;
}

IntMap int_map_set(IntMap map, char *key, int value) {
    for (int i = 0; i < map.count; ++i) {
        if (strcmp(map.keys.data[i], key) == 0) {
            *(map.values.data[i]) = value;
            return map;
        }
    }

    map.count++;

    map.keys.data = realloc(map.keys.data, map.count * sizeof(char *));
    map.values.data = realloc(map.values.data, map.count * sizeof(int *));

    map.keys.data[map.count - 1] = strdup(key);
    map.values.data[map.count - 1] = malloc(sizeof(int));
    *(map.values.data[map.count - 1]) = value;

    map.keys.count = map.count;
    map.values.count = map.count;

    return map;
}

int *int_map_get(IntMap map, char *key) {
    for (int i = 0; i < map.count; ++i) {
        if (strcmp(map.keys.data[i], key) == 0) {
            return map.values.data[i];
        }
    }
    return NULL;
}

IntMap clear_int_map(IntMap map) {
    clear_str_list(map.keys);
    clear_int_list(map.values);
    map.count = 0;
    return map;
}

// ===== MAP UTILS (END)