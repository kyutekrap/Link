#include "./test2.c"
#include <time.h>
#include <stdio.h>

void test(var, char) {
	clock_t __flow_start__, __flow_end__;
	__flow_start__ = clock();
	__flow_end__ = clock();
	printf("[Info] TAG took %f seconds\n", ((double)(__flow_end__-__flow_start__)) / CLOCKS_PER_SEC);
}