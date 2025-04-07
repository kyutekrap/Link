#include <stdio.h>
#include <time.h>

void test2() {
	clock_t __step_start__, __step_end__;
	__step_start__ = clock();
	__step_end__ = clock();
	printf("[Info]: %.2fms elapsed (test2)", ((double)(__step_end__-__step_start__)/CLOCKS_PER_SEC) * 1000);
}

void test(var, char) {
	clock_t __flow_start__, __flow_end__;
	__flow_start__ = clock();
	test2()	__flow_end__ = clock();
	printf("[Info]: %.2fms elapsed (test)", ((double)(__flow_end__-__flow_start__)/CLOCKS_PER_SEC) * 1000);
}