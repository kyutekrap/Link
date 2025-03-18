#include "../include/lexer.h"

int lexer(const char *filename) {
    File fileProps = checkfile(filename);
    if (fileProps.validFile == -1) return INVALID_FILE;

    Env env = readenv(fileProps.root);

    char **files;
    int readDecor = 0;
    if (fileProps.readDecor == -1) {
        files = readinc(fileProps.root);
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) return FILE_NOT_FOUND;

    char *outname = delfile(fileProps.root);
    if (outname[0] == '\0') return C_COMPILE_ERROR;

    char fline[256];
    MacroT macroT = invalid;
    FILE *out = NULL;
    while (fgets(fline, sizeof(fline), file)) {
        if (macroT == 2) {
            macroT = getmacro(fline);
        } else {
            if (out == NULL) {
                out = fopen(outname, "w");
                if (out == NULL) return C_COMPILE_ERROR;
            }
            if (fileProps.readDecor == 0) {
                if (readDecor == 0) {
                    Decorator decorator = getdecor(fline);
                    if (decorator.errCode != 0) {
                        fclose(out);
                        return decorator.errCode;
                    }
                    if (decorator.isDecor == 0) {
                        files = decorator.files;
                    } else if (decorator.isDecor == 2) {
                        continue;
                    } else if (decorator.isDecor == 1) {
                        readDecor = -1;
                        if (env.debug == 0) {
                            fputs(HEADER, out);
                            fputs(MAIN, out);
                            if (macroT == 0) {
                                fputs(FLOW_S, out);
                            } else {
                                fputs(STEP_S, out);
                            }
                        }
                    }
                } else {
                    fputs(fline, out);
                }
            } else {
                if (readDecor == 0) {
                    readDecor = -1;
                    if (env.debug == 0) {
                        fputs(HEADER, out);
                        fputs(MAIN, out);
                        if (macroT == 0) {
                            fputs(FLOW_S, out);
                        } else {
                            fputs(STEP_S, out);
                        }
                    }
                    fputs(fline, out);
                } else {
                    fputs(fline, out);
                }
            }
        }
    }
    fclose(file);
    if (out != NULL) {
        if (readDecor != 0) {
            if (env.debug == 0) {
                if (macroT == 0) {
                    fputs(FLOW_E, out);
                } else {
                    fputs(STEP_E, out);
                }
            }
            fputs(END, out);
        }
        fclose(out);
    } else {
        return LACK_OF_CONTENT;
    }

    if (macroT == 2) return MACRO_NOT_FOUND;

    return SUCCESS;
}