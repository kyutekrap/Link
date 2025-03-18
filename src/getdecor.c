#include "../include/getdecor.h"

// Find and read decorator
Decorator getdecor(char *line) {
    Decorator decorator = {2, 0, NULL};
    size_t fidx = 0;

    KeyT keyT = none;
    char keyword[12] = "";
    size_t idx = 0;
    size_t valueLen = 256;
    char *value = malloc(valueLen * sizeof(char));
    if (value == NULL) {
        decorator.errCode = MEMORY_ALLOCATION_FAILED;
        return decorator;
    }

    size_t vidx = 0;
    char current;

    for (size_t i = 0; i < strlen(line); i++) {
        current = line[i];
        if (isspace(current) && (keyT == 0 || (keyT == 2 && vidx == 0) || keyT == 4)) continue;
        if (keyT == 4) {
            decorator.errCode = ILLEGAL_CHARACTER;
            break;
        }
        
        if (current == '[') {
            if (keyT == 0) {
                keyT = 1;
                decorator.isDecor = 0;
            } else {
                decorator.errCode = BAD_USE_OF_BRACKET;
                break;
            }
        } else if (current == ']') {
            if (keyT != 3) {
                decorator.errCode = BAD_USE_OF_BRACKET;
                break;
            } else {
                keyT = 4;
            }
        } else if (current == '(') {
            if (keyT == 1) {
                keyT = 2;
                keyword[idx] = '\0';
                if (strcmp(keyword, "LocalImport") != 0) {
                    decorator.errCode = UNKNOWN_DECORATOR;
                    break;
                }
            } else {
                decorator.errCode = BAD_USE_OF_PARENTHESIS;
                break;
            }
        } else if (current == ')') {
            if (keyT == 2) {
                if (vidx != 0) {
                    value[vidx] = '\0';
                    decorator.files = realloc(decorator.files, (fidx + 1) * sizeof(char *));
                    if (decorator.files == NULL) {
                        decorator.errCode = MEMORY_ALLOCATION_FAILED;
                        break;
                    }

                    decorator.files[fidx] = malloc((vidx + 1) * sizeof(char));
                    if (decorator.files[fidx] == NULL) {
                        decorator.errCode = MEMORY_ALLOCATION_FAILED;
                        break;
                    }

                    strcpy(decorator.files[fidx], value);
                    fidx++;
                }
                keyT = 3;
            } else {
                decorator.errCode = BAD_USE_OF_PARENTHESIS;
                break;
            }
        } else {
            if (keyT == 0) {
                decorator.isDecor = 1;
                break;
            } else if (keyT == 1) {
                if (idx == 11) {
                    decorator.errCode = UNKNOWN_DECORATOR;
                    break;
                } else {
                    keyword[idx++] = current;
                    keyword[idx] = '\0';
                }
            } else if (keyT == 2) {
                if (current == ',') {
                    if (vidx == 0) {
                        decorator.errCode = ILLEGAL_CHARACTER;
                        break;
                    } else {
                        value[vidx] = '\0';
                        decorator.files = realloc(decorator.files, (fidx + 1) * sizeof(char *));
                        if (decorator.files == NULL) {
                            decorator.errCode = MEMORY_ALLOCATION_FAILED;
                            break;
                        }

                        decorator.files[fidx] = malloc((vidx + 1) * sizeof(char));
                        if (decorator.files[fidx] == NULL) {
                            decorator.errCode = MEMORY_ALLOCATION_FAILED;
                            break;
                        }

                        strcpy(decorator.files[fidx], value);
                        fidx++;
                        value[0] = '\0';
                        vidx = 0;
                    }
                } else {
                    if (vidx == valueLen - 2) {
                        valueLen += 10;
                        value = realloc(value, valueLen * sizeof(char));
                        if (value == NULL) {
                            decorator.errCode = MEMORY_ALLOCATION_FAILED;
                            break;
                        }
                    }
                    value[vidx++] = current;
                }
            } else if (keyT == 3) {
                decorator.errCode = UNCLOSED_DECORATOR;
                break;
            }
        }
    }

    free(value);

    return decorator;
}