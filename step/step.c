#include "./step.h"

char *step_s() {
    char *result = NULL;
    result = join_str(result, "\tclock_t __step_start__, __step_end__;\n");
    result = join_str(result, "\t__step_start__ = clock();\n");
    return result;
}

char *step_e(char *namespace) {
    char *result = NULL;
    result = join_str(result, "\t__step_end__ = clock();\n");

    char *info_line = NULL; 
    info_line = join_str(info_line, "\tprintf(\"[Info]: %.2fms elapsed (");
    info_line = join_str(info_line, namespace);
    info_line = join_str(info_line, ")\", ((double)(__step_end__-__step_start__)/CLOCKS_PER_SEC) * 1000);");

    result = join_str(result, info_line);
    result = join_str(result, "\n");
    return result;
}