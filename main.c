#include "./include/lexer.h"
#include "./macro/errcode.h"
#include <stdio.h>

int main() {
    int res = lexer("C:\\Users\\user\\Link\\example\\test.link");
    switch(res) {
        case SUCCESS:
            printf("SUCCESS");
            break;
        case INVALID_FILE:
            printf("INVALID_FILE");
            break;
        case FILE_NOT_FOUND:
            printf("FILE_NOT_FOUND");
            break;
        case MACRO_NOT_FOUND:
            printf("MACRO_NOT_FOUND");
            break;
        case C_COMPILE_ERROR:
            printf("C_COMPILE_ERROR");
            break;
        default:
            break;
    }
    return res;
}