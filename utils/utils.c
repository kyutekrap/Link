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

// ===== STRING UTILS (END)

// ===== STRUCT UTILS (START)

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

// ===== STRUCT UTILS (END)