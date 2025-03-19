#include "../include/delfile.h"

// Make subdir and delete output file
DelFile delfile(char *root) {
    DelFile delFile = {0, ""};

    int rootLen = strlen(root);
    delFile.file = malloc((rootLen+12) * sizeof(char));
    if (delFile.file == NULL) {
        delFile.errCode = MEMORY_ALLOCATION_FAILED;
        return delFile;
    }

    char subdir[rootLen+6];
    strcpy(subdir, root);
    strcat(subdir, ".link");

    struct stat st = {0};
    if (stat(subdir, &st) == -1) {
        if (_mkdir(subdir) == -1) {
            delFile.errCode = FOLDER_CREATION_FAILED;
            return delFile;
        }
    }

    strcpy(delFile.file, root);
    strcat(delFile.file, ".link\\out.c");
    remove(delFile.file);

    return delFile;
}