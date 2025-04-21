#ifndef pad_H
#define pad_H

typedef enum {
    Left,
    Right
} Direction;

#include <stdlib.h>
#include <string.h>

void *pad(Direction dir, void *data, int count, size_t element_size)

#endif