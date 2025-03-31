#include "../include/checkfile.h"

// Detect if .link
// Detect if main.link (Otherwise, read decorators)
// Detect project root
File checkfile(const char *filename) {
    File file = {0, 0, "", 0, ""};

    int fnameLen = 4;
    char *fname = malloc(fnameLen * sizeof(char));
    if (fname == NULL) {
        file.errCode = MEMORY_ALLOCATION_FAILED;
        return file;
    }
    int fnameIdx = 0;
    char ext[5] = "";
    int extIdx = 0;
    char current;
    for (int counter = 0; counter < strlen(filename); counter++) {
        current = filename[counter];
        if (current == '\\') {
            fnameIdx = 0;
        } else if (current != '.' && ext[0] == '\0') {
            fname[fnameIdx] = current;
            fnameIdx++;
            if (fnameLen == fnameIdx-1) {
                fnameLen++;
                fname = realloc(fname, fnameLen * sizeof(char));
                if (fname == NULL) {
                    file.errCode = MEMORY_ALLOCATION_FAILED;
                    return file;
                }
            }
        } else if (extIdx < 5) {
            ext[extIdx] = current;
            extIdx++;
        } else {
            ext[0] = '\0';
            break;
        }
    }
    fname[fnameIdx] = '\0';
    ext[extIdx] = '\0';

    if (strcmp(ext, ".link") == 0) {
        int len = strlen(filename) - strlen(fname) - strlen(ext);
        file.root = malloc((len + 1) * sizeof(char));
        if (file.root == NULL) {
            file.errCode = MEMORY_ALLOCATION_FAILED;
            return file;
        }
        for (size_t counter=0; counter <= len; counter++) {
            file.root[counter] = filename[counter];
        }
        file.root[len] = '\0';
    } else {
        file.validFile = -1;
    }
    if (strcmp(fname, "main") == 0) {
        file.readDecor = -1;
    }
    file.fname = fname;
    
    return file;
}