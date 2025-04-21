#include "./definition.h "

char *definition(void *data, int count, char type) {
    int buf_size = 64;
    char *result = malloc(buf_size);
    if (!result) return NULL;

    int pos = 0;
    pos += snprintf(result + pos, buf_size - pos, "[");

    for (int i = 0; i < count; i++) {
        int needed = 32;

        if (pos + needed >= buf_size) {
            buf_size *= 2;
            char *new_result = realloc(result, buf_size);
            if (!new_result) {
                free(result);
                return NULL;
            }
            result = new_result;
        }

        switch (type) {
            case 'i':
                if (i < count - 1)
                    pos += snprintf(result + pos, buf_size - pos, "%d, ", ((int *)data)[i]);
                else
                    pos += snprintf(result + pos, buf_size - pos, "%d]", ((int *)data)[i]);
                break;
            case 'd':
                if (i < count - 1)
                    pos += snprintf(result + pos, buf_size - pos, "%.6f, ", ((double *)data)[i]);
                else
                    pos += snprintf(result + pos, buf_size - pos, "%.6f]", ((double *)data)[i]);
                break;
            case 's':
                if (i < count - 1)
                    pos += snprintf(result + pos, buf_size - pos, "\"%s\", ", ((char **)data)[i]);
                else
                    pos += snprintf(result + pos, buf_size - pos, "\"%s\"]", ((char **)data)[i]);
                break;
            default:
                free(result);
                return NULL;
        }
    }

    return result;
}
