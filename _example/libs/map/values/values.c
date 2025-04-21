#include "./values.h"

char *values(void *data) {
    int buf_size = 64;
    char *result = malloc(buf_size);
    if (!result) return NULL;

    strcpy(result, "[");

    Map *map = (Map *)data;
    int pos = strlen(result);

    for (int i = 0; i < map->count - 1; i++) {
        int needed = strlen((char **)map->keys[i]) + 3;

        if (pos + needed >= buf_size) {
            buf_size *= 2;
            char *new_result = realloc(result, buf_size);
            if (!new_result) {
                free(result);
                return NULL;
            }
            result = new_result;
        }

        pos += snprintf(result + pos, buf_size - pos, "%s, ", ((char **)map->values)[i]);
    }

    snprintf(result + pos, buf_size - pos, "%s]", ((char **)map->values)[map->count - 1]);

    return result;
}