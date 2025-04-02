#include "../include/getcomm.h"

// Check if line is a comment
GetComm getcomm(char *fline) {
    GetComm getComm = {0, -1, commNeutral};

    char current;
    char previous = '\0';
    for (int i=0; i<strlen(fline)-1; i++) {
        current = fline[i];
        if (previous == '/') {
            if (current == '/') {
                getComm.isComment = 0;
            } else if (current == '*') {
                getComm.isComment = 0;
                getComm.direction = commStart;
            }
            break;
        } else {
            if (isspace(current)) continue;
            if (current == '/') previous = current;
            else break;
        }
    }

    if (getComm.isComment == 0) return getComm;

    int left = 0, right = strlen(fline) - 1;
    char flineCopy[right+1];
    while (left < right) {
        char temp = fline[left];
        flineCopy[left] = fline[right];
        flineCopy[right] = temp;
        left++;
        right--;
    }
    flineCopy[right+1] = '\0';
    previous = '\0';
    for (int i=0; i<strlen(flineCopy)-1; i++) {
        current = flineCopy[i];
        if (previous == '/') {
            if (current == '*') {
                getComm.isComment = 0;
                getComm.direction = commEnd;
            }
            break;
        } else {
            if (isspace(current)) continue;
            if (current == '/') previous = '/';
            else break;
        }
    }

    return getComm;
}