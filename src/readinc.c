#include "../include/readinc.h"

// Read include.txt if exists
ReadInc readinc(char *root) {
    ReadInc readInc = {NULL, 0};
    size_t count = 0;

    int fileLen = strlen(root) + 12;
    char *filename = malloc(fileLen * sizeof(char));
    if (filename == NULL) {
        readInc.errCode = MEMORY_ALLOCATION_FAILED;
        return readInc;
    }
    strcpy(filename, root);
    strcat(filename, "include.txt\0");

    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            if (isspace(line[0])) continue;
            int fLen = fileLen + strlen(line) + 1;
            char *fname = malloc(fLen * sizeof(char));
            if (fname == NULL) {
                readInc.errCode = MEMORY_ALLOCATION_FAILED;
                fclose(file);
                return readInc;
            }

            strcpy(fname, root);
            strcat(fname, line);
            fname[fLen-1] = '\0';
            readInc.files = realloc(readInc.files, (count + 1) * sizeof(char *));
            if (readInc.files == NULL) {
                readInc.errCode = MEMORY_ALLOCATION_FAILED;
                fclose(file);
                return readInc;
            }
            readInc.files[count] = fname;
            count++;
        }
    } else {
        readInc.errCode = FILE_NOT_FOUND;
        return readInc;
    }
    fclose(file);
    
    return readInc;
}