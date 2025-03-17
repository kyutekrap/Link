#ifndef STEP_E_H
#define STEP_E_H

#define STEP_E "\
__end__=clock();\
printf(\"[INFO] TAG took %f seconds\n\"((double)(end-start))/CLOCKS_PER_SEC);\
"

#endif