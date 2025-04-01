#include<time.h>
#include<stdio.h>
void test(){clock_t __start__,__end__;__start__=clock();@import("./test2.link")
@debug(true)
@param(name, str)
// 
/*
*/__end__=clock();printf("[INFO] TAG took %f seconds
"((double)(end-start))/CLOCKS_PER_SEC);}