#include "../include/getmacro.h"

// Find macros at opening (flow, step)
GetMacro getmacro(const char *fline) {
    GetMacro getMacro = {invalid, 0};

    char macro[6] = "";
    int macroIdx = 1;
    char current;
    for (int counter = 0; counter < sizeof(fline); counter++) {
        current = fline[counter];
        if (macro[0] == '\0') {
            if (isspace(current)) {
                continue;
            } else {
                macro[0] = current;
            }
        } else {
            if (macroIdx < 5) {
                macro[macroIdx] = current;
                macroIdx++;
            } else if (macroIdx == 5) {
                macro[macroIdx] = '\0';
                if (strcmp(macro, "#flow") == 0) {
                    getMacro.macroT = flow;
                } else if (strcmp(macro, "#step") == 0) {
                    getMacro.macroT = step;
                }
                break;
            }
        }
    }

    return getMacro;
}