#include "./utils.h"

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