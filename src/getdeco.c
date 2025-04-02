#include "../include/getdeco.h"

GetDeco getdeco(char *fline) {
    GetDeco getDeco = {0, 0, 0, NULL};
    char decorator[7] = {0};
    size_t decoratorIdx = 0;
    char *variable = NULL;
    size_t variableIdx = 0;
    size_t variableLen = 0;
    size_t status = 0;
    size_t line_len = strlen(fline);

    for (size_t i = 0; i <= line_len; i++) {
        char current = i < line_len ? fline[i] : '\0';
        
        if (i < line_len && isspace(current)) continue;

        switch (status) {
            case 0:
                if (current != '@') {
                    getDeco.isDeco = -1;
                    goto cleanup;
                }
                status++;
                break;

            case 1:
                if (isalpha(current)) {
                    if (decoratorIdx < sizeof(decorator)-1) {
                        decorator[decoratorIdx++] = current;
                    } else {
                        getDeco.errCode = UNDEFINED_DECORATOR;
                        goto cleanup;
                    }
                } 
                else if (current == '(') {
                    decorator[decoratorIdx] = '\0';

                    if (strcmp(decorator, "debug") == 0) {
                        getDeco.decoType = debug;
                        variableLen = 6;
                    } 
                    else if (strcmp(decorator, "import") == 0) {
                        getDeco.decoType = import;
                        variableLen = PATH_MAX;
                    } 
                    else if (strcmp(decorator, "param") == 0) {
                        getDeco.decoType = param;
                        variableLen = 40;
                    } 
                    else {
                        getDeco.errCode = UNDEFINED_DECORATOR;
                        goto cleanup;
                    }

                    variable = malloc(variableLen);
                    if (!variable) {
                        getDeco.errCode = MEMORY_ALLOCATION_FAILED;
                        goto cleanup;
                    }
                    status++;
                } 
                else {
                    getDeco.errCode = UNDEFINED_DECORATOR;
                    goto cleanup;
                }
                break;

            case 2:
                if (current == ')' || current == '\0') {
                    variable[variableIdx] = '\0';
                    status++;
                } else {
                    if (variableIdx >= variableLen-1) {
                        if (getDeco.decoType == debug) {
                            getDeco.errCode = UNDEFINED_DECORATOR;
                            goto cleanup;
                        } 
                        else if (getDeco.decoType == import) {
                            getDeco.errCode = FILENAME_TOO_LONG;
                            goto cleanup;
                        } 
                        else {
                            size_t newLen = variableLen + 40;
                            char *temp = realloc(variable, newLen);
                            if (!temp) {
                                getDeco.errCode = MEMORY_ALLOCATION_FAILED;
                                goto cleanup;
                            }
                            variable = temp;
                            variableLen = newLen;
                        }
                    }
                    variable[variableIdx++] = current;
                }
                break;

            case 3:
                if (getDeco.decoType == debug) {
                    if (strcmp(variable, "true") != 0 && strcmp(variable, "false") != 0) {
                        getDeco.errCode = UNDEFINED_DECORATOR;
                        goto cleanup;
                    }
                } 
                else if (getDeco.decoType == param) {
                    int typing = -1;
                    size_t commas = 0;
                    
                    for (size_t cnt = 0; cnt < variableIdx; cnt++) {
                        char var = variable[cnt];
                        
                        if (cnt != 0 && commas == 0 && isspace(var)) {
                            getDeco.errCode = UNDEFINED_DECORATOR;
                            goto cleanup;
                        }
                        else if (var == ',' && commas == 0) {
                            commas = 1;
                            if (cnt+1 < variableIdx && !isspace(variable[cnt+1])) {
                                getDeco.errCode = UNDEFINED_DECORATOR;
                                goto cleanup;
                            }
                        }
                        else if (var == ',' && commas == 1) {
                            getDeco.errCode = UNDEFINED_DECORATOR;
                            goto cleanup;
                        }
                        else if (commas == 1 && !isspace(var)) {
                            typing = 0;
                        }
                        else if (typing == 0 && isspace(var)) {
                            getDeco.errCode = UNDEFINED_DECORATOR;
                            goto cleanup;
                        }
                    }
                    
                    if (commas != 1) {
                        getDeco.errCode = UNDEFINED_DECORATOR;
                        goto cleanup;
                    }
                } 
                else {
                    if (variableIdx >= 2 && 
                        (variable[0] == '"' || variable[0] == '\'') && 
                        variable[variableIdx-1] == variable[0]) {
                        memmove(variable, variable + 1, variableIdx - 2);
                        variable[variableIdx - 2] = '\0';
                        variableIdx -= 2;
                    }
                }
                
                getDeco.value = strdup(variable);
                if (!getDeco.value) {
                    getDeco.errCode = MEMORY_ALLOCATION_FAILED;
                    goto cleanup;
                }
                
                goto cleanup;
        }
    }

cleanup:
    if (getDeco.value != variable) {
        free(variable);
    }
    return getDeco;
}