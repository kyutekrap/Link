#include "./test2.c"
#include <stdio.h>
#include <time.h>

void test(var, char) {
	clock_t __flow_start__, __flow_end__;
	__flow_start__ = clock();
	__flow_end__ = clock();
	printf("[Info]: %.2fms elapsed (test)", ((double)(__flow_end__-__flow_start__)/CLOCKS_PER_SEC) * 1000);
}