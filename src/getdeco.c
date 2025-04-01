#include "../include/getdeco.h"

// Read and interpret decorators
GetDeco getdeco(char *fline) {
    GetDeco getDeco = {0, 0, 0};

    char decorator[7];
    size_t decoratorIdx;
    char *variable;
    size_t variableIdx, variableLen;

    size_t status = 0;
    char current;
    for (size_t i=0; i<strlen(fline)-1; i++) {
        current = fline[i];
        if (isspace(current)) continue;
        switch (status) {
            case 0:
                if (current != '@') {
                    getDeco.isDeco = -1;
                    return getDeco;
                }
                current ++;
                break;
            case 1:
                if (isalpha(current)) {
                    if (decoratorIdx < 7) {
                        decorator[decoratorIdx] = current;
                        decoratorIdx ++;
                    } else {
                        getDeco.errCode = UNDEFINED_DECORATOR;
                        return getDeco;
                    }
                } else if (current == '(') {
                    if (strcmp(decorator, "debug") == 0) {
                        getDeco.decoType = debug;
                        variableLen = 6;
                    } else if (strcmp(decorator, "import") == 0) {
                        getDeco.decoType = import;
                        variableLen = PATH_MAX;
                    } else if (strcmp(decorator, "param") == 0) {
                        getDeco.decoType = param;
                        variableLen = 40;
                    } else {
                        getDeco.errCode = UNDEFINED_DECORATOR;
                        return getDeco;
                    }
                    variable = malloc(variableLen * sizeof(char));
                    current ++;
                } else {
                    getDeco.errCode = UNDEFINED_DECORATOR;
                    return getDeco;
                }
                break;
            case 2:
                if (current == ')') current ++;
                else {
                    if (variableIdx == variableLen-1) {
                        if (getDeco.decoType == debug) {
                            getDeco.errCode = UNDEFINED_DECORATOR;
                            return getDeco;
                        } else if (getDeco.decoType == import) {
                            getDeco.errCode = FILENAME_TOO_LONG;
                            return getDeco;
                        } else {
                            variableLen += 40;
                            variable = realloc(variable, variableLen * sizeof(char));
                            variable[variableIdx] = current;
                            variableIdx ++;
                        }
                    } else {
                        variable[variableIdx] = current;
                        variableIdx ++;
                    }
                }
                break;
            case 3:
                if (getDeco.decoType == debug) {
                    if (!(strcmp(variable, "true") == 0 || strcmp(variable, "false") == 0)) {
                        getDeco.errCode = UNDEFINED_DECORATOR;
                        return getDeco;
                    }
                } else if (getDeco.decoType == param) {
                    int typing = -1;
                    size_t commas = 0;
                    char var;
                    for (size_t cnt=0; cnt<variableIdx-1; cnt++) {
                        var = variable[variableIdx];
                        if (cnt != 0 && commas == 0 && isspace(var)) {
                            getDeco.errCode = UNDEFINED_DECORATOR;
                            return getDeco;
                        }
                        else if (var == ',' && commas == 0) commas = 1;
                        else if (var == ',' && commas == 1) {
                            getDeco.errCode = UNDEFINED_DECORATOR;
                            return getDeco;
                        }
                        else if (commas == 1 && !isspace(var)) typing = 0;
                        else if (typing == 0 && isspace(var)) {
                            getDeco.errCode = UNDEFINED_DECORATOR;
                            return getDeco;
                        }
                    }
                } else {
                    if ((variable[0] == '"' || variable[0] == '\'') 
                    && (variable[variableIdx-1] == '"' || variable[variableIdx-1] == '\'')) {
                        char new_str[variableIdx+1];
                        strncpy(new_str, variable + 1, variableIdx - 2);
                        new_str[variableIdx - 2] = '\0';
                        variable = new_str;
                    }
                }
                getDeco.value = variable;
                return getDeco;
        }
    }
    
    return getDeco;
}