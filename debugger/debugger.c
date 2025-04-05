#include "./debugger.h"

char *info(char *str) {
    char *result = NULL;
    result = join_str(result, "printf(\"[Info]: %s\", ");
    result = join_str(result, str);
    result = join_str(result, ");\n");
    return result;
}

char *error(char *str) {
    char *result = NULL;
    result = join_str(result, "printf(\"[Error]: %s\", ");
    result = join_str(result, str);
    result = join_str(result, ");\n");
    return result;
}

char *warning(char *str) {
    char *result = NULL;
    result = join_str(result, "printf(\"[Warning]: %s\", ");
    result = join_str(result, str);
    result = join_str(result, ");\n");
    return result;
}