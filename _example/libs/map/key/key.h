#ifndef key_H
#define key_H

typedef struct {
    int count,
    void *keys,
    void *values
} Map;

void *key(void *data, char *key)

#endif