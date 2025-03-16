#include "../include/lexer.h"

int lexer(const char *filename) {
    File fileProps = checkfile(filename);
    if (fileProps.validFile == -1) return INVALID_FILE;

    Env env = readenv(fileProps.root);

    FILE *file = fopen(filename, "r");
    if (file == NULL) return FILE_NOT_FOUND;

    char fline[256];
    MacroT macroT = invalid;
    FILE *out = NULL;
    while (fgets(fline, sizeof(fline), file)) {
        if (fline[0] == '\0') continue;
        if (macroT == 2) {
            macroT = getmacro(fline);
        } else {
            if (out == NULL) {
                char *outname = mkfile(fileProps.root);
                if (outname[0] == '\0') return C_COMPILE_ERROR;
                out = fopen(outname, "w");
                if (out == NULL) return C_COMPILE_ERROR;
                if (env.debug == 0) {
                    fputs(HEADER, out);
                    if (macroT == 0) {
                        fputs(FLOW_S, out);
                    } else {
                        fputs(STEP_S, out);
                    }
                }
                fputs(MAIN, out);
            }
            fputs(fline, out);
        }
    }
    fclose(file);
    if (out != NULL) {
        fputs("}", out);
        fclose(out);
    }

    if (macroT == 2) return MACRO_NOT_FOUND;

    return SUCCESS;
}