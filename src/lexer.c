#include "../include/lexer.h"

int lexer(char *filename) {
    File fileProps = checkfile(filename);
    if (fileProps.errCode != 0) return fileProps.errCode;
    if (fileProps.validFile == -1) return INVALID_FILE;

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
    LexerStatus current = 0;
    int isComment = -1;
    int isDebug = -1;
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
                    current ++;
                }
                break;
            case 1:
                getComm = getcomm(fline);
                if (getComm.isComment == 0) {
                    if (getComm.direction == 1) isComment = 0;
                    else if (getComm.direction == 2) isComment = -1;
                    continue;
                }
                GetDeco getDeco = getdeco(fline);
                if (getDeco.errCode != 0) {
                    fclose(out);
                    return getDeco.errCode;
                }
                if (getDeco.isDeco == 0) {
                    if (getDeco.decoType == import) {
                        fprintf(out, "#include \"%s.c\"\n", getDeco.value);
                        char combined_path[PATH_MAX];
                        char absolute_path[PATH_MAX];
                        snprintf(combined_path, sizeof(combined_path), "%s%s", fileProps.root, getDeco.value);
                        if (_fullpath(absolute_path, combined_path, PATH_MAX) == NULL) {
                            return FILE_NOT_FOUND;
                        }
                        int res = lexer(absolute_path);
                        if (res != 0) {
                            fclose(out);
                            return res;
                        }
                    } else if (getDeco.decoType == debug) {
                        if (strcmp(getDeco.value, "true") == 0) isDebug = 0;
                    } else {

                    }
                } else {
                    if (isDebug == 0) {
                        fputs(HEADER, out);
                    }
                    fprintf(out, "void %s(){", fileProps.fname);
                    if (isDebug == 0) {
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
        if (isDebug == 0) {
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