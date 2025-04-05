#define HEADER "\
#include <time.h>\n\
#include <stdio.h>\n\
\n"

#define FLOW_S "\
\tclock_t __flow_start__, __flow_end__;\n\
\t__flow_start__ = clock();\n\
"

#define FLOW_E "\
\t__flow_end__ = clock();\n\
\tprintf(\"[Info] TAG took %f seconds\\n\", ((double)(__flow_end__-__flow_start__)) / CLOCKS_PER_SEC);\n\
"

#define STEP_S "\
\tclock_t __step_start__, __step_end__;\n\
\t__step_start__ = clock();\n\
"

#define STEP_E "\
\t__step_end__ = clock();\n\
\tprintf(\"[Info] TAG took %f seconds\\n\", ((double)(__step_end__-__step_start__)) / CLOCKS_PER_SEC);\n\
"
