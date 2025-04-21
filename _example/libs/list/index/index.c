#include "./index.h"

void *index(void *data, int count, char type) {
    switch (type) {
        case 'i':
            return &((int *)data)[count];
        case 'd':
            return &((double *)data)[count];
        case 's':
            return ((char **)data)[count];
        default:
            return NULL;
    }
}