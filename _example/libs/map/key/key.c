#include "./key.h"

void *key(void *data, char *key) {
    Map *map = (Map *)data;
    for (int i = 0; i < map->count; i++) {
        if (strcmp(((char **)map->keys)[i], key) == 0) {
            return ((void **)map->values)[i];
        }
    }

    return NULL;
}