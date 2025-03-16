#include "../include/mkfile.h"

// Create subdir and compressed file
char *mkfile(char *root) {
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
    return out;
}