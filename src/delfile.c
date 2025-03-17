#include "../include/delfile.h"

// Make subdir and delete output file
char *delfile(char *root) {
    int rootLen = strlen(root);
    char subdir[rootLen+6];
    char *out = malloc((rootLen+12) * sizeof(char));
    strcpy(subdir, root);
    strcat(subdir, ".link\0");
    struct stat st = {0};
    if (stat(subdir, &st) == -1) {
        if (_mkdir(subdir) == -1) {
            return out;
        }
    }
    strcpy(out, root);
    strcat(out, ".link\\out.c\0");
    remove(out);
    return out;
}