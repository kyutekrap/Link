#ifndef FLOW_E_H
#define FLOW_E_H

#define FLOW_E "\
__end__=clock();\
printf(\"[INFO] TAG took %f seconds\n\"((double)(end-start))/CLOCKS_PER_SEC);\
"

#endif