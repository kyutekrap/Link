#include "./debugger.h"

char *info(char *str) {
    const char *prefix = "printf(\"[Info]: ";
    const char *suffix = "\");\n";

    int len = strlen(prefix) + strlen(str) + strlen(suffix) + 1;
    char *result = malloc(len);
    if (!result) return NULL;

    strcpy(result, prefix);
    strcat(result, str);
    strcat(result, suffix);

    return result;
}

char *error(char *str) {
    const char *prefix = "printf(\"[Error]: ";
    const char *suffix = "\");\n";

    int len = strlen(prefix) + strlen(str) + strlen(suffix) + 1;
    char *result = malloc(len);
    if (!result) return NULL;

    strcpy(result, prefix);
    strcat(result, str);
    strcat(result, suffix);

    return result;
}

char *warning(char *str) {
    const char *prefix = "printf(\"[Warning]: ";
    const char *suffix = "\");\n";

    int len = strlen(prefix) + strlen(str) + strlen(suffix) + 1;
    char *result = malloc(len);
    if (!result) return NULL;

    strcpy(result, prefix);
    strcat(result, str);
    strcat(result, suffix);

    return result;
}