#include "./include/lexer.h"
#include "./macro/errcode.h"
#include <stdio.h>

int main() {
    int res = lexer("C:\\Users\\user\\Link\\example\\src\\test.link");
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
        case LACK_OF_CONTENT: 
            printf("LACK_OF_CONTENT");
            break;
        case BAD_USE_OF_BRACKET:
            printf("BAD_USE_OF_BRACKET");
            break;
        case BAD_USE_OF_PARENTHESIS:
            printf("BAD_USE_OF_PARENTHESIS");
            break;
        case UNKNOWN_DECORATOR:
            printf("UNKNOWN_DECORATOR");
            break;
        case UNCLOSED_DECORATOR:
            printf("UNCLOSED_DECORATOR");
            break;
        case ILLEGAL_CHARACTER:
            printf("ILLEGAL_CHARACTER");
            break;
        case MEMORY_ALLOCATION_FAILED:
            printf("MEMORY_ALLOCATION_FAILED");
            break;
        default:
            break;
    }
    return res;
}