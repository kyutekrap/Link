#include "../include/getfiles.h"

// Helper function to recursively get files
void getfiles_recursive(const char *directory, GetFiles *getFiles, size_t *fidx) {
    struct dirent *dp;
    DIR *dir = opendir(directory);

    if (!dir) return;

    while ((dp = readdir(dir)) != NULL) {
        char path[1024];
        struct stat statbuf;

        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) continue;

        snprintf(path, sizeof(path), "%s%s", directory, dp->d_name);

        if (stat(path, &statbuf) == -1) continue;

        if (S_ISDIR(statbuf.st_mode)) {
            char newPath[strlen(path)+2];
            snprintf(newPath, sizeof(newPath), "%s\\", path);
            getfiles_recursive(newPath, getFiles, fidx);
        } else {
            size_t len = strlen(path);
            if (len < 5) continue;

            if (strcmp(path + len - 5, ".link") == 0) {
                char **temp = realloc(getFiles->files, (*fidx + 1) * sizeof(char *));
                if (!temp) {
                    getFiles->errCode = MEMORY_ALLOCATION_FAILED;
                    break;
                }
                getFiles->files = temp;

                getFiles->files[*fidx] = malloc(len + 1);
                if (!getFiles->files[*fidx]) {
                    getFiles->errCode = MEMORY_ALLOCATION_FAILED;
                    break;
                }
                strcpy(getFiles->files[*fidx], path);
                (*fidx)++;
            }
        }
    }

    closedir(dir);
}

// Get files from root and subdirectories
GetFiles getfiles(const char *filename) {
    GetFiles getFiles = {0, NULL, 0};
    size_t fidx = 0;

    getfiles_recursive(filename, &getFiles, &fidx);

    getFiles.fileCnt = fidx;
    return getFiles;
}