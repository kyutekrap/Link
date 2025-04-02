#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include "../macro/errcode.h"
#include "../macro/flow_e.h"
#include "../macro/flow_s.h"
#include "../macro/header.h"
#include "../macro/step_e.h"
#include "../macro/step_s.h"
#include "../include/checkfile.h"
#include "../include/getmacro.h"
#include "../include/delfile.h"
#include "../include/getdeco.h"
#include "../include/isempty.h"
#include "../include/getfiles.h"
#include "../include/getcomm.h"

enum LexerStatus {
    macro,
    decor,
    content
};
typedef enum LexerStatus LexerStatus;

int lexer(char *filename);

#endif