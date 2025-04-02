#include "../include/lexer.h"

int lexer(char *filename) {
    File fileProps = checkfile(filename);
    if (fileProps.errCode != 0) return fileProps.errCode;
    if (fileProps.validFile == -1) return INVALID_FILE;

    Env env = readenv(fileProps.root);
    if (env.errCode != 0) return env.errCode;

    FILE *file = fopen(filename, "r");
    if (file == NULL) return FILE_NOT_FOUND;

    DelFile delFile = delfile(filename);
    char *outname = delFile.file;
    if (delFile.errCode != 0) return delFile.errCode;
    if (outname[0] == '\0') return C_COMPILE_ERROR;

    FILE *out = fopen(outname, "w");
    if (out == NULL) return C_COMPILE_ERROR;

    char fline[256];
    MacroT macroT = invalid;
    LexerStatus current = none;
    int isComment = -1;
    while (fgets(fline, sizeof(fline), file)) {
        if (isComment == 0 || isempty(fline) == 0) continue;
        switch(current) {
            case 0:
                GetComm getComm = getcomm(fline);
                if (getComm.isComment == 0) {
                    if (getComm.direction == 1) isComment = 0;
                    else if (getComm.direction == 2) isComment = -1;
                    continue;
                }
                GetMacro getMacro = getmacro(fline);
                if (getMacro.errCode != 0) return getMacro.errCode;
                macroT = getMacro.macroT;
                if (macroT != 2) {
                    if (fileProps.readDecor == 0) current = decor;
                    else {
                        if (env.debug == 0) {
                            fputs(HEADER, out);
                        }
                        fprintf(out, "void %s(){", fileProps.fname);
                        if (env.debug == 0) {
                            if (macroT == 0) {
                                fputs(FLOW_S, out);
                            } else {
                                fputs(STEP_S, out);
                            }
                        }
                        current = content;
                    }
                }
                break;
            case 1:
                getComm = getcomm(fline);
                if (getComm.isComment == 0) {
                    if (getComm.direction == 1) isComment = 0;
                    else if (getComm.direction == 2) isComment = -1;
                    continue;
                }
                Decorator decorator = getdecor(fline);
                if (decorator.errCode != 0) {
                    fclose(out);
                    return decorator.errCode;
                }
                if (decorator.isDecor == 0) {
                    fprintf(out, "#include \"%s.c\"\n", decorator.files[0]);
                    char combined_path[PATH_MAX];
                    char absolute_path[PATH_MAX];
                    snprintf(combined_path, sizeof(combined_path), "%s%s", fileProps.root, decorator.files[0]);
                    if (_fullpath(absolute_path, combined_path, PATH_MAX) == NULL) {
                        return FILE_NOT_FOUND;
                    }
                    int res = lexer(absolute_path);
                    if (res != 0) return res;
                } else if (decorator.isDecor == 1) {
                    if (env.debug == 0) {
                        fputs(HEADER, out);
                    }
                    fprintf(out, "void %s(){", fileProps.fname);
                    if (env.debug == 0) {
                        if (macroT == 0) {
                            fputs(FLOW_S, out);
                        } else {
                            fputs(STEP_S, out);
                        }
                    }
                    fputs(fline, out);
                    current ++;
                }
                break;
            case 2:
                fputs(fline, out);
                break;
        }
    }
    fclose(file);

    if (current == 2) {
        if (env.debug == 0) {
            if (macroT == 0) {
                fputs(FLOW_E, out);
            } else {
                fputs(STEP_E, out);
            }
        }
        fputs("}", out);
    } else {
        fprintf(out, "void %s(){}", fileProps.fname);
    }
    fclose(out);

    if (macroT == 2) return MACRO_NOT_FOUND;

    if (strcmp(fileProps.fname, "main") == 0) {
        GetFiles getFiles = getfiles(fileProps.root);
        for (size_t i=0; i<getFiles.fileCnt; i++) {
            if (strcmp(getFiles.files[i], filename) == 0) continue;
            int res = lexer(getFiles.files[i]);
            if (res != 0) return res;
        }
    }

    return SUCCESS;
}