#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "../macro/errcode.h"
#include "../macro/flow_e.h"
#include "../macro/flow_s.h"
#include "../macro/header.h"
#include "../macro/step_e.h"
#include "../macro/step_s.h"
#include "../macro/main.h"
#include "../macro/end.h"
#include "../include/checkfile.h"
#include "../include/readenv.h"
#include "../include/getmacro.h"
#include "../include/delfile.h"
#include "../include/readinc.h"

int lexer(const char *filename);

#endif