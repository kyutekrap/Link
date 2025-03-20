#include "../include/delfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DelFile delfile(const char *filename) {
    DelFile delFile = {0, NULL};

    int fileLen = strlen(filename);
    delFile.file = malloc((fileLen-3) * sizeof(char));
    if (delFile.file == NULL) {
        delFile.errCode = MEMORY_ALLOCATION_FAILED;
        return delFile;
    }

    strcpy(delFile.file, filename);
    delFile.file[fileLen-4] = 'c';
    delFile.file[fileLen-3] = '\0';

    remove(delFile.file);

    return delFile;
}