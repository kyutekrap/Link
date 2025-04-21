#include "./pad.h"

void *pad(Direction dir, void *data, int count, size_t element_size) {
    void *new_array = malloc(count * element_size + count * element_size);
    if (!new_array) return NULL;

    memset(new_array, 0, count * element_size);

    if (dir == Left) {
        memcpy((char *)new_array + count * element_size, data, element_size * count);
    } else if (dir == Right) {
        memcpy(new_array, data, element_size * count);
    }

    return new_array;
}