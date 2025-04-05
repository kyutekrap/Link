#include "./flow.h"

char *flow_s() {
    char *result = NULL;
    result = join_str(result, "\tclock_t __flow_start__, __flow_end__;\n");
    result = join_str(result, "\t__flow_start__ = clock();\n");
    return result;
}

char *flow_e(char *namespace) {
    char *result = NULL;
    result = join_str(result, "\t__flow_end__ = clock();\n");

    char *info_line = NULL; 
    info_line = join_str(info_line, "\tprintf(\"[Info]: %.2fms elapsed (");
    info_line = join_str(info_line, namespace);
    info_line = join_str(info_line, ")\", ((double)(__flow_end__-__flow_start__)/CLOCKS_PER_SEC) * 1000);");

    result = join_str(result, info_line);
    result = join_str(result, "\n");
    return result;
}