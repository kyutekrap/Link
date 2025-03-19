#include "../include/isempty.h"

// Check is string empty
int isempty(char *ln) {
    while (*ln) {
        if (!isspace((unsigned char)*ln)) return -1;
        ln++;  
    }
    return 0;
}