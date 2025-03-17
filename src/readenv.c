#include "../include/readenv.h"

// Find env file, if none, default to DEBUG=TRUE
Env readenv(char *root) {
    Env env = {0};

    int fileLen = strlen(root) + 5;
    char *filename = malloc(fileLen * sizeof(char));
    strcpy(filename, root);
    strcat(filename, ".env");
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        char keyword[6] = "";
        int keywordIdx = 1;
        char value[6] = "";
        int valueIdx = 0;
        char line[11];
        char current;
        while (fgets(line, sizeof(line), file)) {
            for (int counter = 0; counter < sizeof(line); counter++) {
                current = line[counter];
                if (keyword[0] == '\0') {
                    if (isspace(current)) {
                        continue;
                    } else {
                        keyword[0] = current;
                    }
                } else {
                    if (keywordIdx < 5) {
                        keyword[keywordIdx] = current;
                        keywordIdx++;
                    } else {
                        keyword[keywordIdx] = '\0';
                        if (current == '=' || isspace(current)) {
                            continue;
                        } else {
                            value[valueIdx] = current;
                            valueIdx++;
                        }
                    }
                }
                value[valueIdx] = '\0';
                if (strcmp(keyword, "DEBUG") == 0 && strcmp(value, "FALSE") == 0) {
                    env.debug = -1;
                }
            }
        }
    }
    fclose(file);
    free(filename);
    
    return env;
}