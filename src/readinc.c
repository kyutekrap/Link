#include "../include/readinc.h"

// Read include.txt if exists
char **readinc(char *root) {
    char **files = NULL;
    size_t count = 0;

    int fileLen = strlen(root) + 12;
    char *filename = malloc(fileLen * sizeof(char));
    strcpy(filename, root);
    strcat(filename, "include.txt\0");

    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            if (isspace(line[0])) continue;
            int fLen = fileLen + strlen(line) + 1;
            char *fname = malloc(fLen * sizeof(char));
            strcpy(fname, root);
            strcat(fname, line);
            fname[fLen-1] = '\0';
            files = realloc(files, (count + 1) * sizeof(char *));
            files[count] = fname;
            count++;
        }
    }
    fclose(file);
    
    return files;
}